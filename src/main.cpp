#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Common.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glu.h>

//
// qutemol on wxWidgets MAIN 
//
// (on DevCpp, download and install "wxWidgets 2.6.1 unicode" DevPak)
//
////////////////////////////////////////////////////////////////////

#define SnapButton 1
#define OpenButton 2
#define QuitButton 3

#include <wx/dnd.h>
#include <wx/notebook.h>

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/image.h"
#include "wx/stdpaths.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include <iostream>
#include <wx/cmdline.h>
#include <wx/statline.h>

#include "main.h"

#ifndef __WXMSW__
#    include <OpenGL/glu.h>
#    include <glu.h>
#else
#  include <GL/glu.h>
#endif

//#include "sample.xpm"

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

#include "gifSave.h"

#include "MyTab.h"

#include "saveSnapDialog.h"

#include "progress.h"

#include "AtomColor.h"

extern vcg::Trackball track;
extern vcg::Trackball lightTrack;

extern Mol mol;

bool mustDoHQ=false;

wxBitmap *LoadPngImage(wxString st){
#ifdef __DARWIN__
	wxString basePath = wxStandardPaths::Get().GetResourcesDir();
  wxBitmap* res=new wxBitmap(basePath+_T("/image/")+st+_T(".png"),wxBITMAP_TYPE_PNG);
#else
  wxBitmap* res=new wxBitmap(_T("image/")+st+_T(".png"),wxBITMAP_TYPE_PNG);
#endif  
  return res;
}

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(SnapButton,  MyFrame::OnSaveSnap)
    EVT_MENU(OpenButton,  MyFrame::OnOpenFile)
    EVT_MENU(QuitButton,  MyFrame::OnQuit)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than for a
// static object, which is "constructed" too early, and may cause problems)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the obj application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("QuteMol"),
                                 wxPoint(50, 50), wxSize(750, 500));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

class MyDropTarget : public wxFileDropTarget
{
public:
    MyDropTarget(MyFrame *f) { frame=f; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames)
    {
      frame->OpenFile(filenames[0]);
    }

private:
    MyFrame *frame;
};


// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // set the frame icon
    //SetIcon(wxICON(sample));

#ifdef __WXMSW__
    SetIcon(wxIcon(_T("sample")));
#endif

    wxBoxSizer *main_sizer = new wxBoxSizer( wxHORIZONTAL );

    notebook = new wxNotebook(this, wxID_ANY);
    mytab=new MyTab(notebook);
    notebook->AddPage(mytab, _T("Settings"));

    main_sizer->Add(notebook, 0, wxEXPAND | wxALL, 0);

    canvas = new MyCanvas(this);
    main_sizer->Add(canvas, 1, wxEXPAND | wxALL, 0);

    SetSizer( main_sizer );
    main_sizer->SetSizeHints( this );

    SetDropTarget(new MyDropTarget(this));
    
    int het=GetCurrentHetatm();
    int atm=GetCurrentAtm();
    
    wxString status;
    status.Printf(_T("Hetatms: %d. Atoms: %d"),het,atm);

    // create a status bar just some managed pixels
    CreateStatusBar(2);
    SetStatusText(status);

    hardSettings.OnStart();
    
    UpdateAtomColor();
    
    CSIZE=hardSettings.SNAP_SIZE;
    
}


void MyFrame::OpenFile(wxString fn) {
  InitQuteMol((const char*)fn.mb_str(wxConvUTF8));
  canvas-> mol_loaded=true;
  int het=GetCurrentHetatm();
  int atm=GetCurrentAtm();

  wxString status;
  status.Printf(_T("Hetatms: %d. Atoms: %d"),het,atm);
  SetStatusText(status);

  SetStatusText(fn,1);
  mytab->UpdateMol();
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog *dialog = new wxFileDialog(this, _T("Open PDB file"), _T(""), _T(""), _T("PDB files (*.pdb)|*.pdb"), wxFD_OPEN);
  if (dialog->ShowModal() == wxID_OK) OpenFile(dialog->GetPath());
}

void MyFrame::OnIdle(wxIdleEvent& event) {
  if (canvas->mol_loaded) {
    if (track.IsMoving() || lightTrack.IsMoving()) {
      canvas->Refresh(false);
    } else {
      if (mustDoHQ) {
        canvas->Refresh(false);
        mustDoHQ=false;
      }
    }
  }
}


void MyFrame::OnSaveSnap(wxCommandEvent& WXUNUSED(event))
{
    SaveSnapDialog *dialog = new saveSnapDialog(this);
    if (dialog->ShowModal() == wxID_OK) {

      int AAMult=1;
      if (hardSettings.SNAP_ANTIALIAS) AAMult=2;

      int sx=hardSettings.SNAP_SIZE*AAMult;
      int sy=hardSettings.SNAP_SIZE*AAMult;

      wxString fn=dialog->GetPath();
      wxString ext;

      int mode=dialog->GetMode();

      switch (mode) {
          case 0:
          case 1: {
            bool alpha=(mode==0);
            Byte* snap;

            if (fn.Find('.')==wxNOT_FOUND) {
               if (alpha) ext=_T(".png");
               else ext=_T(".jpg");
               fn+=ext;
            }

            StartProgress("Saving snap!", 2);

            snap = GetSnapshot(sx,sy, alpha);

            if (!snap) {
               wxMessageBox(CANNOT_SAVE, _T("OpenGL problems?"), wxOK | wxICON_EXCLAMATION, this);
               EndProgress();
               return;
            }

            StepProgress();

            if (alpha) {
              if (AAMult!=1) downsample2x2(snap, sx, sy);
              PNGSaveWithAlpha((const char*)fn.mb_str(wxConvUTF8),snap,sx/AAMult,sy/AAMult,1);
            }
            else {
              //if (AAMult!=1) downsample2x2NoAlpha(snap, sx, sy);
              //SaveImageJPG((const char*)fn.mb_str(wxConvUTF8),snap,sx/AAMult,sy/AAMult);
            };
           break;
          }
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
              N=hardSettings.GIF_6SIDES_N_FRAMES;
              totalTime=hardSettings.GIF_6SIDES_MSEC/1000.0;
              subStepTime=hardSettings.GIF_6SIDES_PAUSE/1000.0;
            }

            if (fn.Find('.')==wxNOT_FOUND) {
               ext=_T(".gif");
               fn+=ext;
            }

            StartProgress("Saving GIF", N);

            Byte* snap;
            
            for (int i=0; i<N; i++) {
              StepProgress();

              if (hardSettings.GIF_ANIMATION_MODE==0) {
                 float angle=i*360.0/N;
                 canvas->SetViewRot(angle, hardSettings.GIF_GIF_ROT_SIDEVIEW_ANGLE);
              }
              if (hardSettings.GIF_ANIMATION_MODE==1) {
                 float angle=sin(i*M_PI*2.0/N)*hardSettings.GIF_INSP_ANGLE;
                 canvas->SetViewInsp(angle);
              }
              if (hardSettings.GIF_ANIMATION_MODE==2) {
                 canvas->SetViewSide(i, N, hardSettings.GIF_6SIDES_PAUSE);
              }

              canvas->drawNow(hardSettings.STILL_QUALITY);

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
            gifw.Save((const char*)fn.mb_str(wxConvUTF8));
            break;
          }
      }
      EndProgress();
    }
}
