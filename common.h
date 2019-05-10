#ifndef COMMON_H
#define COMMON_H
#define MAX_SERVOS_NUMBER   6
#define PAGE0   0
#define PAGE1   1
#define PAGE2   2
#define PAGE3   3
#define MAX_PAGES   4
#define MAX_CONTROL_PORTS    32  /* control port numbers in SSC-32 */
#define DEFAULT_INI_NAME    "qrobot.prj"

#define MIN_ANGLE       0.0f
#define MAX_ANGLE       180.0f
#define CENTER_ANGLE    90.0f
#define DEFAULT_MIN_PW     500
#define DEFAULT_MAX_PW     2500
#define DEFAULT_CENTER_PW  1500

class MainWindow;
extern MainWindow* gMainWnd;
class SystemData;
extern SystemData* gSystem;
#endif // COMMON_H
