
#if !defined(DIALOG_FILESELECTION_H)
#define DIALOG_FILESELECTION_H

class DialogFileSelection{
 public :
  DialogFileSelection(){}
  virtual ~DialogFileSelection(){}

  enum Mode{
    AnyFile //- The name of a file, whether it exists or not.
    , ExistingFile //- The name of a single existing file.
    , Directory //- The name of a directory. Both files and directories are displayed.
    , DirectoryOnly //- The name of a directory. The file dialog will only display directories.
    , ExistingFiles //- The names of zero or more existing files.
  };
  enum Type{
    Open,
    Save
  };
};

#endif
