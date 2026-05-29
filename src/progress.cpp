#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef WIN32
#include <windows.h>
#endif
#include "qopengl.h"

#include "wx/wxprec.h"
#include <wx/progdlg.h>

#include "progress.h"

wxProgressDialog *globalProgress=NULL;

bool UpdateProgress(int i){
  bool res=globalProgress->Update(i);
  if (!res) EndProgress();
  return res;
}

void StartProgress(char* str, int N){
  EndProgress();
  wxString st; st.Printf("QuteMol: %s...",str);
  globalProgress= new wxProgressDialog(
    //_T("QuteMol"), st,
    st,  _T(""),
    N, NULL, wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT);
}

void EndProgress(){
  if (globalProgress) {
    globalProgress->Destroy();
    globalProgress=NULL;
  }
}