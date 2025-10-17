
#if !defined(PWD__INCLUDED_) && defined __MINGW32__
#define PWD_H__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdlib.h>

  //#include <sys/types.h>
typedef int gid_t;
typedef int uid_t;

struct passwd {
  char *pw_name;
  char *pw_gecos;
  uid_t pw_uid;
  gid_t pw_gid;
  char *pw_dir;
  char *pw_shell;
  char *pw_passwd;
};


static char slash[] = "/";
static char shell[] = "sh";
static char default_login[] = "user";
char *
getlogin(void)
{
  char *p;

  p = getenv("USER");
  if (!p)
    p = getenv("LOGNAME");
  if (!p)
    p = getenv("USERNAME");
  if (!p)
    p = default_login;
  return p;
}
/* return something like a uid.  */
int
getuid ()
{
  char *p;

  p = getenv("UID");
  if (p)
    return atoi(p);
  return 0;			/* every user is a super user ... */
}

gid_t getgid(void)
{
  return 0;
}

struct passwd *
getpwuid(uid_t uid)
{
  static struct passwd rv;
  rv.pw_uid = getuid();
  if (uid != rv.pw_uid)
    return 0;
  rv.pw_name = getlogin();
  rv.pw_gecos = getlogin();
  rv.pw_gid = getgid();
  rv.pw_dir = getenv("HOME");
  if (rv.pw_dir == 0) {
    if ( getenv("HOMEDRIVE") ) {
      // AEHH BILLY's WORLD
      static char pw_dir[200];
      rv.pw_dir = pw_dir;
      sprintf(rv.pw_dir, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
    }
  }
  if (rv.pw_dir == 0)
    rv.pw_dir = slash;
  rv.pw_shell = getenv("SHELL");
  if (rv.pw_shell == 0)
    rv.pw_shell = getenv("COMSPEC");
  if (rv.pw_shell == 0)
    rv.pw_shell = shell;
  return &rv;
}


/* struct passwd *	getpwuid(uid_t _uid); */
/* struct passwd *	getpwnam(const char *_name); */

/* struct passwd	*getpwent(void); */
/* void		setpwent(void); */
/* void		endpwent(void); */

#ifdef __cplusplus
}
#endif

#endif
