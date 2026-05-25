#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
//
// qutemol on wxWidgets MAIN 
//
// (on DevCpp, download and install "wxWidgets 2.6.1 unicode" DevPak)
//
////////////////////////////////////////////////////////////////////

#define SnapButton 1
#define OpenButton 2
#define QuitButton 3



#include <GL/glew.h>


#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/image.h"
#include "wx/stdpaths.h"

//#include <iostream>
#include <wx/cmdline.h>
#include <wx/statline.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "main.h"
#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/glu.h>
#  else
#    include <glu.h>
#  endif
#else
#  include <GL/glu.h>
#endif

//#include "sample.xpm"

typedef unsigned char Byte;
typedef unsigned int uint;

#include "CgUtil.h"

#include <math.h>

#include <vector>

#include <vcg/space/point3.h>
#include <vcg/space/color4.h>

#include "gen_normal.h"
#include <wrap/gui/trackball.h>

using namespace vcg;
using namespace std;

#include "CubeMapSamp.h"
#include "OctaMapSamp.h"
#include "Mol.h"
//#include "AOgpu.h"

#include "MyCanvas.h"

#include "ShadowMap.h"
#include "HardSettings.h"

#include "Common.h"

#include "MyTab.h"

#include "saveSnapDialog.h"

#include "progress.h"

#include "AtomColor.h"

extern vcg::Trackball track;
extern vcg::Trackball lightTrack;

extern Mol mol;

bool mustDoHQ=false;

#include <wx/dnd.h>
#include <wx/notebook.h>

wxStopWatch sw;


#include "gifSave.h"

// defined in pngSave
bool PNGSaveWithAlpha( const char * filename, const Byte * data, int sx, int sy, int reverse = 0);
void downsample2x2(Byte * data, int sx, int sy);
void downsample2x2NoAlpha(Byte * data, int sx, int sy);



wxBitmap *LoadPngImage(wxString st){
#ifdef __DARWIN__
	wxString basePath = wxStandardPaths::Get().GetResourcesDir();
  wxBitmap* res=new wxBitmap(basePath+_T("/image/")+st+_T(".png"),wxBITMAP_TYPE_PNG);
#else
  wxBitmap* res=new wxBitmap(_T("image/")+st+_T(".png"),wxBITMAP_TYPE_PNG);
#endif  
  //wxBitmap* res=new wxBitmap(st,wxBITMAP_TYPE_PNG_RESOURCE);
  
  // Marke all black pixels as transparent (useful for Windows NT only)
  //res->SetMask(new wxMask(*res, wxColor(0,0,0)));

  return res;
}

void MyTab::SceneChanged(){
  ((TestGLCanvas*)frame2redraw)->SceneChanged();
}


void TestGLCanvas::SceneChanged(){
  mustDoHQ=false;
  Refresh(false);
}

void StartTime(){
  sw.Start();
}

long int TakeTime(FILE *f , char *st){
  long int delta=sw.Time();
  fprintf(f,"%5ldmsec: %s\n",delta,st);
  //globaltime=timen;
  return delta;
}
/*long int TakeTotalTime(){
  long int timen=getTicks(), delta=timen-startingtime;
  printf("------------------\nTotal time: %5dmsec\n",delta);
  globaltime=timen;
  return delta;
}*/


long int getTicks(){
  return 0;
}

void MyToolbar::SetTitleText(wxString s){
#ifndef __DARWIN__
  titleText->SetLabel(s);
  titleTextSizer->Layout();
#endif
}
  
class MyDropTarget : public wxFileDropTarget
{
public:
    MyDropTarget(MyFrame *_parent) { parent = _parent;  }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,  const wxArrayString& filenames){
      parent->OnReadFile(filenames[0]);
    };

private:
    MyFrame *parent;
};

void MyToolbar::OnDrag(wxMouseEvent &event){
   
   if ( event.LeftDClick()) {
      parent->Maximize( !(parent->IsMaximized()) );
   }

   static int omx, omy;
   int mx, my;
   mx = event.GetX();
   my = event.GetY();
   
   if ( event.Dragging() ){
     if (!(parent->IsMaximized())) {
      CaptureMouse();
      wxPoint cur=parent->GetPosition();
      int dx=mx-omx;
      int dy=my-omy;
      cur.x+=dx;
      cur.y+=dy;
      omx=mx-dx;
      omy=my-dy;
      parent->SetPosition( cur ); 
     }
   } else {
     omx=mx; omy=my;
     ReleaseMouse();
  }
}

wxNotebook *notebook;

void MyToolbar::UpdateGearsIcon(){
  UpdateGearsIcon( mol.DoingAO() );
}

void MyToolbar::UpdateGearsIcon(bool b){
  if (gearPresent==b) return;
  if (!b) gearS->Detach(gear); else gearS->Add(gear);
  gear->Show(b);
  gearS->Layout();
  gearPresent=b;
/*
  if (b) 
  gear->Raise();
  else
  gear->Lower();*/
}

MyToolbar::MyToolbar(wxTopLevelWindow *_parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size, 
        long style,
        const wxString& name): wxPanel(_parent, id, pos, size, style, name){
          
  parent=_parent;
  
  // let's build notebook
  /*wxNotebook **/notebook = new wxNotebook(
    this, id, pos, size, 
    0,/*style/*|wxNO_FULL_REPAINT_ON_RESIZE|wxNB_TOP,*/
    name);
    
  for (int i=0; i<MyTab::Count(); i++) {
    //if (i==2)
    notebook->AddPage(new MyTab(notebook,i), MyTab::Title(i), i==0 );
      parent->SetBackgroundColour(notebook->GetBackgroundColour());
  }
  
#ifndef __DARWIN__    
  wxBitmapButton *snapButton = new wxBitmapButton(this, SnapButton, 
                                *LoadPngImage(_T("snap_off2")), wxDefaultPosition, wxSize(48,48),0);
  snapButton->SetBitmapSelected(*LoadPngImage(_T("snap_on")));  
    
  wxBitmapButton *openButton = new wxBitmapButton(this, OpenButton, 
                                *LoadPngImage(_T("open_off2")),wxDefaultPosition, wxSize(48,48),0);
  openButton->SetBitmapSelected(*LoadPngImage(_T("open_on")));  
#else
  wxBitmapButton *snapButton = new wxBitmapButton(this, SnapButton, 
                                *LoadPngImage(_T("snap_noborder")), wxDefaultPosition, wxSize(48,48),0);
  wxBitmapButton *openButton = new wxBitmapButton(this, OpenButton, 
                                *LoadPngImage(_T("open_noborder")),wxDefaultPosition, wxSize(48,48),0);

#endif
  wxBitmapButton *quitButton = new wxBitmapButton(this, QuitButton, 
                           *LoadPngImage(_T("quitoff")),
                           wxDefaultPosition, wxSize(15,16) , 0);
  quitButton->SetBitmapSelected(*LoadPngImage(_T("quiton")));  
  
  wxStaticBitmap *logo = new wxStaticBitmap(this, wxID_ANY, *LoadPngImage(_T("qutemolsm")), wxDefaultPosition );

  gear = new wxStaticBitmap(this, wxID_ANY, *LoadPngImage(_T("gear2")), wxDefaultPosition );
  
  wxSizer *sizerButtons = new wxBoxSizer( wxVERTICAL );
  sizerButtons->Add(openButton, 0,  wxALL, 1);
  sizerButtons->Add(snapButton, 0,  wxALL, 1);

  titleText = new wxStaticText(this, wxID_ANY, wxT(""));
  titleText->Disable();
  
#ifndef __DARWIN__
  wxSizer *linesizer1 = new wxBoxSizer(wxVERTICAL);

  linesizer1->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);
  linesizer1->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);
  linesizer1->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);

  wxSizer *linesizer2 = new wxBoxSizer(wxVERTICAL);
  linesizer2->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);
  linesizer2->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);
  linesizer2->Add(new wxStaticLine(this),0,wxALL| wxEXPAND, 1);
 
  titleTextSizer = new wxBoxSizer(wxHORIZONTAL);
  titleTextSizer->Add(3,3,        0,  wxALL, 3); 
  titleTextSizer->Add(linesizer1, 1,  wxALL|wxALIGN_CENTER_VERTICAL, 0); 
  titleTextSizer->Add(titleText,  0,  wxALL|wxALIGN_CENTER_VERTICAL, 0); 
  titleTextSizer->Add(linesizer2, 20,  wxALL|wxALIGN_CENTER_VERTICAL, 0); 
  titleTextSizer->Add(3,3,        0,  wxALL, 3); // spacer
  titleTextSizer->Add(quitButton, 0,  wxALL|wxALIGN_CENTER_VERTICAL, 2); 

#else
  quitButton->Hide();
  titleText->Hide();
#endif



  wxSizer *imgsizer = new wxBoxSizer(wxHORIZONTAL);
  imgsizer->Add(22,22, 0); 
  //imgsizer->Add(1,1, 10); 
  imgsizer->Add(logo, 1,  wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 0); // LOGO
  //imgsizer->Add(1,1, 9); 
  
  gearS = new wxBoxSizer(wxVERTICAL);
  gearS->Add(22,22,1);
  gearS->Add(gear);
  gearPresent=true;
  
  imgsizer->Add(gearS, 0,  wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 2); // LOGO
  
  wxSizer *topRsizer = new wxBoxSizer(wxVERTICAL);
#ifndef __DARWIN__
	  topRsizer->Add(titleTextSizer,0, wxALL| wxEXPAND, 0);
#endif 
	topRsizer->Add(imgsizer,1, wxALL| wxEXPAND, 0);
  
  topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerButtons,  0, wxALL, 5);
  topsizer->Add(topRsizer,     1, wxALIGN_TOP| wxALL| wxEXPAND, 0);

  
  wxSizer *globalsizer = new wxBoxSizer(wxVERTICAL);
  globalsizer->Add(topsizer, 0,  wxALL| wxEXPAND, 0);
  //globalsizer->Add(5, 5,       0,  wxALL, 0); // spacer
  globalsizer->Add(notebook, 1,  wxALL|wxEXPAND, 0);
  
  SetSizer(globalsizer);
  
  UpdateGearsIcon(false);
  
}


// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    hardSettings.OnStart();
    
    cgSettings.SetDefaults(); // <-- quick hack (solves wrong constructor order): 
      
    //if (!wxApp::OnInit()) return false;

    // questo per caricare salvare PNG...
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    
    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, wxT("QuteMol"),
//        wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE //wxRESIZE_BORDER
#ifdef __DARWIN__
        wxDefaultPosition, wxDefaultSize,wxDEFAULT_FRAME_STYLE| wxRESIZE_BORDER
#else
        wxDefaultPosition, wxDefaultSize,wxRESIZE_BORDER
#endif
        );
        
    /* Make a menubar */
    
    /*
    // ...or, maybe NOT
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    fileMenu->Append(wxID_ABOUT, wxT("A&bout"));
    fileMenu->Append(wxID_OPEN, wxT("O&pen"));
    
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, wxT("&File"));
    
    frame->SetMenuBar(menuBar);
    */

    frame->SetCanvas( 
      new TestGLCanvas(
        frame, wxID_ANY, wxPoint(0,0), wxSize( winx, winy ),   wxNO_BORDER //wxSUNKEN_BORDER
      )
    );

    // onle TestGLCanvas to process idles...
    wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
    frame->SetExtraStyle(wxWS_EX_PROCESS_IDLE );
    
    MyTab::frame2redraw=frame->GetCanvas();
    
    frame->SetToolbar(  
      new MyToolbar(
        frame, wxID_ANY, wxPoint(winx,0), wxDefaultSize,   
       // wxNO_BORDER //wxSUNKEN_BORDER
        wxDEFAULT_FRAME_STYLE 
       |
//                      wxNO_FULL_REPAINT_ON_RESIZE |
                      wxCLIP_CHILDREN |
                      wxTAB_TRAVERSAL
      )
    );
    
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(frame->GetCanvas(),      1, wxGROW|wxSHAPED); 
    sizer->Add(frame->GetToolbar(),      0, wxGROW);
    frame->SetSizer(sizer);

    int tbsize=frame->GetToolbar()->GetEffectiveMinSize().x;
    frame->GetToolbar()->SetSize(tbsize,winy);
    frame->GetToolbar()->Layout();
   
    //frame->SetMinSize(wxSize(tbsize+200,200));
    frame->SetClientSize(wxSize(winx+tbsize,winy));

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
      { wxCMD_LINE_PARAM, NULL, NULL, "filename.pdb:(molecule to be drawn)", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_OPTION, "a", NULL, "filename.art: optional Atom Render Table", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_SWITCH, "v", NULL, "don't start, show version name", wxCMD_LINE_VAL_NONE, 0 },
      { wxCMD_LINE_NONE }
    };
        //wxString cext=fn.AfterLast('.');
        //if (cext.CmpNoCase(ext[0])||cext.CmpNoCase(ext[1])) fn=fn.BeforeLast('.');
        //fn=fn+_T('.')+ext[ jj ];
        
        switch (jj) {
          case 0:
          case 1: 
            if (!useTransp) {
              StartProgress("Saving snap!", 2);
              wxImage snapi(sx,sy,snap);
              if (AAMult!=1) snapi=snapi.Scale(sx/AAMult,sy/AAMult);
              UpdateProgress(1);
              wxBitmap(snapi.Mirror(false)).SaveFile(
                fn,
                (jj==0)?wxBITMAP_TYPE_PNG:wxBITMAP_TYPE_JPEG
              );
              UpdateProgress(2);
              EndProgress();
            }
            else {
              if (AAMult!=1) downsample2x2(snap, sx, sy);
              PNGSaveWithAlpha((const char*)fn.mb_str(wxConvUTF8),snap,sx/AAMult,sy/AAMult,1);
            };
           break;
          case 2: {
            GifWrapper gifw;
            int N;
            double totalTime;
            double subStepTime=0;
            double startTime;startTime=hardSettings.GIF_INITIAL_PAUSE/1000.0;
            if (hardSettings.GIF_ANIMATION_MODE==0) {
              N=hardSettings.GIF_ROT_N_FRAMES;
              totalTime=hardSettings.GIF_ROT_MSEC/1000.0;
            } 
            if (hardSettings.GIF_ANIMATION_MODE==1) {
              N=hardSettings.GIF_INSP_N_FRAMES;
              totalTime=hardSettings.GIF_INSP_MSEC/1000.0;
            } 
            if (hardSettings.GIF_ANIMATION_MODE==2) {
              N=hardSettings.GIF_6SIDES_N_FRAMES*6;
              totalTime=hardSettings.GIF_6SIDES_MSEC*6.0/1000.0;
              subStepTime=hardSettings.GIF_6SIDES_PAUSE/1000.0;
            } 
            
            StartProgress("Saving GIF", N);         
            for (int i=0; i<N; i++) {
              if (!UpdateProgress(i)) break;
              setAniStep(double(i)/N);
              snap = GetSnapshot(sx,sy, false);
              
              if (!snap) {
                 wxMessageBox(CANNOT_SAVE, _T("OpenGL problems?"), wxOK | wxICON_EXCLAMATION, this);
                 EndProgress();
                 return;
              }
              
              if (AAMult!=1) downsample2x2NoAlpha(snap, sx, sy);
              gifw.AddFrame(snap, sx/AAMult, sy/AAMult, 
                totalTime/N 
                + ( (i==0)?startTime:0) 
                + ( (i%(N/6)==0)?subStepTime:0) 
              ); 
            } 
            stopAni();
            gifw.Save( (const char*)fn.mb_str(wxConvUTF8) );
            EndProgress();
            } break;
        }
        
      }
   }
                  
}


void MyFrame::OnReadFile (wxString filename)
{
   if (mol.ReadPdb((const char*)filename.mb_str(wxConvUTF8) )) {
       if ((mol.natm==0) && (mol.nhetatm!=0)) geoSettings.showHetatm=true;
       MyTab::EnableGeom();
       UpdateShadowmap();
       wxString name( mol.GetMolName(), wxConvUTF8 ); 
       SetTitle(  name.BeforeLast('.')+ _T(" - QuteMol") );
       m_tb->SetTitleText(name);
       geoSettings.Apply();

       // redo shaders, as texture size could have changed 
       cgSettings.UpdateShaders();

   } else {
      wxMessageBox(wxString::FromAscii(QAtom::lastReadError), _T(":-("), wxOK | wxICON_EXCLAMATION, this);
      m_tb->SetTitleText();
   }
   
  // remake shaders cos texture size could have changed...

   
   //theText->LoadFile(openFileDialog->GetFilename());
   //SetStatusText(GetCurrentFilename(), 0);      
}


void MyFrame::OnOpenFile (wxCommandEvent & event)
{ 
  static const wxString FILETYPES = _T( 
                   "Protein Data Bank molecule (pdb, vdb)|*.pdb;*.vdb|"
                   "All files|*.*"
                  );
  wxFileDialog * openFileDialog =
       new wxFileDialog ( this,
                          wxString(_T("Open file")),
                          wxString(), // def path
                          wxString(), 
                          FILETYPES,
                          wxOPEN | wxFILE_MUST_EXIST/*| wxCHANGE_DIR*/,
                          wxDefaultPosition);

   if (openFileDialog->ShowModal() == wxID_OK) {
     OnReadFile( openFileDialog->GetPath() );
   }
/* wxString filename=openFileDialog->GetFilename();
   wxString dir=openFileDialog->GetDirectory();*/
}

void MyFrame::OnIdle(wxIdleEvent& event){
  if (mol.DoingAO()) {
    m_tb->UpdateGearsIcon(true);
    if (mol.PrepareAOstep()) {
      m_canvas->SceneChanged();
      event.RequestMore(false); 
      m_tb->UpdateGearsIcon(false);
      return;
    }
    else event.RequestMore(true);      
  } else {
    event.RequestMore(false); 
    if (hardSettings.STILL_QUALITY!=hardSettings.MOVING_QUALITY)
    if (!m_canvas->shownHQ) {
      m_canvas->shownHQ=true;
      mustDoHQ=true;
      m_canvas->Refresh(false);
    }
    m_tb->UpdateGearsIcon(false);
  }
}

void TestGLCanvas::OnMouse( wxMouseEvent& event )
{
    
    static bool useLightTrack=false;

    if ( event.m_rightDown || event.m_leftDown || event.m_middleDown ) {
      if (!HasCapture()) CaptureMouse();
    }
    else if (HasCapture()) ReleaseMouse();
    
    useLightTrack=event.m_rightDown;

    bool consumed=false;
    if (useLightTrack) {
      MovingLightMode=true;
      if (wxConsumeTrackBallEvent( event, lightTrack)) {
        SetFocus();
        consumed=true;
      }
    } else {
      MovingLightMode=false;
      if (wxConsumeTrackBallEvent( event, track)) {
        SetFocus();
        consumed=true;
      }
    }
    
    if ((consumed) && (!event.ButtonUp()) && (!event.ButtonDown())) SceneChanged();
    
/*   if ( event.m_rightUp ) {
      useLightTrack=false;
    }*/

}

void TestGLCanvas::InitGL()
{
  initGl();
}

