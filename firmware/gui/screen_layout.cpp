#include "screen_layout.h"
#include "safecast_wirish_types.h"
// types are: ITEM_TYPE_MENU, ITEM_TYPE_LABEL, ITEM_TYPE_SELECTNUM, ITEM_TYPE_SELECTION, ITEM_TYPE_VARLABEL, ITEM_TYPE_GRAPH

__FLASH__ screen screens_layout[SCREEN_COUNT] = {

// Screen 0 - main screen
{
 4,
 {
   { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT"      , 255}, 
   { ITEM_TYPE_MENU       , 1, 1, "Current Readings", 4},
   { ITEM_TYPE_MENU       , 2, 2, "Reports"         , 5},
   { ITEM_TYPE_MENU       , 3, 3, "Settings"        , 0}
//   { ITEM_TYPE_MENU_ACTION, 0, 4, "Sleep" }
 }
},

// Screen 1 - Current readings screen
{
  6,
  {
    { ITEM_TYPE_HEAD        , 0,  0, "CPMDEADINT", 255}, 
    { ITEM_TYPE_BIGVARLABEL , 0, 26, "CPMDEAD"   , 255},
    { ITEM_TYPE_LABEL       ,89, 60, " CPM"      , 255},
    { ITEM_TYPE_BIGVARLABEL , 0, 70, "SIEVERTS"  , 255},
    { ITEM_TYPE_LABEL       ,79,104, "\x80Sv/h"  , 255},
    { ITEM_TYPE_SMALLLABEL  ,8,120, "SIEVERTS ESTIMATED",255}
  }
},

// Screen 2 - Reports menu
{
  4,
  {
    { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT",255 }, 
    { ITEM_TYPE_MENU       , 4, 1, "Graphs",1 },
    { ITEM_TYPE_MENU       ,15, 2, "Accumulate/Avg",6 },
//    { ITEM_TYPE_MENU_ACTION, 0, 3, "Serial Transfer",1},
    { ITEM_TYPE_MENU_ACTION, 0, 3, "QR Transfer",7}
  }
},

// Screen 3 - Settings menu
{
  5,
  {
    { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT", 255 }, 
    { ITEM_TYPE_MENU       ,16, 1, "Interface" , 8 },
    { ITEM_TYPE_MENU       ,17, 2, "Geiger"    , 9 },
    { ITEM_TYPE_MENU       , 9, 3, "Time/Date" , 13 },
    { ITEM_TYPE_MENU       ,18, 4, "Version"   , 14 }
  }
},


// Screen 4 - Graph display
{
  3,
  {
    { ITEM_TYPE_HEAD    , 0 , 0  , "CPMDEADINT"    , 255 }, 
    { ITEM_TYPE_GRAPH   , 4 , 110, "RECENTDATA"    , 255 },
    { ITEM_TYPE_LABEL   ,255, 112, "Last 2 minutes", 255 }
  }
},

//Screen 5 - Calibration Confirmation
{
  4,
  {
    { ITEM_TYPE_HEAD , 0 , 0  , "CPMDEADINT"       , 255},
    { ITEM_TYPE_MENU , 6 , 3  , "       OK       " , 255},
    { ITEM_TYPE_MENU , 3 , 4  , "     Cancel     " , 255},
    { ITEM_TYPE_LABEL,255, 16 , "Are you sure?"    , 255}
  }
},

//Screen 6 - Calibration Wait 1
{
  4,
  {
    { ITEM_TYPE_LABEL,255, 32, "Waiting for 15s" , 255 },
    { ITEM_TYPE_LABEL,255, 64, "Please expose"   , 255 },
    { ITEM_TYPE_LABEL,255, 80, "to source."      , 255 },
    { ITEM_TYPE_DELAY, 60,100,"DELAYA\0 16,7"    , 255 } // 16 second delay, then go to screen 7
  } 
},

//Screen 7 - Calibration Wait 2
{
  2,
  {
    { ITEM_TYPE_LABEL,255, 32, "Acquiring, 30s" , 255 },
    { ITEM_TYPE_DELAY, 60,100,"DELAYB\0 31,8"   , 255 } // 31 second delay
  } 
}, 
    

//Screen 8 - Calibration
{
  8,
  {
    { ITEM_TYPE_VARLABEL   , 0 , 0  , "FIXEDSV"  , 255 },
    { ITEM_TYPE_VARNUM     , 20, 50 , "CAL1"     , 255 },
    { ITEM_TYPE_VARNUM     , 56, 50 , "CAL2"     , 255 },
    { ITEM_TYPE_VARNUM     , 74, 50 , "CAL3"     , 255 },
    { ITEM_TYPE_VARNUM     , 92, 50 , "CAL4"     , 255 },
    { ITEM_TYPE_MENU_ACTION,  0,   7, "Save"     , 2 },
    { ITEM_TYPE_ACTION     ,  0,   0, "CALIBRATE", 255 }
  }
},

//Screen 9 - Set time/date
{
  3,
  {
    { ITEM_TYPE_HEAD,             0, 0, "CPMDEADINT"   , 255 }, 
    { ITEM_TYPE_MENU,            10, 1, "Set Time"     , 15 },
    { ITEM_TYPE_MENU,            11, 2, "Set Date/Zone", 16 }
  }
},

//Screen 10 - Set time
{
  8,
  {
    { ITEM_TYPE_HEAD       , 0 , 0 , "CPMDEADINT" , 255 }, 
    { ITEM_TYPE_VARNUM     , 10, 50, "TIMEHOUR1"  , 255 },
    { ITEM_TYPE_VARNUM     , 32, 50, "TIMEHOUR2"  , 255 },
    { ITEM_TYPE_VARNUM     , 54, 50, "TIMEMIN1"   , 255 },
    { ITEM_TYPE_VARNUM     , 76, 50, "TIMEMIN2"   , 255 },
    { ITEM_TYPE_VARNUM     , 98, 50, "TIMESEC1"   , 255 },
    { ITEM_TYPE_VARNUM     ,118, 50, "TIMESEC2"   , 255 },
    { ITEM_TYPE_MENU_ACTION,  0,  7, "SaveTime"   , 2 }
  }
},

//Screen 11 - Set date
{
  10,
  {
    { ITEM_TYPE_HEAD       , 0 , 0  , "CPMDEADINT", 255 }, 
    { ITEM_TYPE_VARNUM     , 10, 50 , "DATEMON1"  , 255 },
    { ITEM_TYPE_VARNUM     , 32, 50 , "DATEMON2"  , 255 },
    { ITEM_TYPE_VARNUM     , 54, 50 , "DATEDAY1"  , 255 },
    { ITEM_TYPE_VARNUM     , 76, 50 , "DATEDAY2"  , 255 },
    { ITEM_TYPE_VARNUM     , 98, 50 , "DATEYEAR1" , 255 },
    { ITEM_TYPE_VARNUM     ,118, 50 , "DATEYEAR2" , 255 },
    { ITEM_TYPE_MENU_ACTION,  0,   7, "SaveDate"  , 2 },
    { ITEM_TYPE_LABEL      , 0 ,  20, "MM/DD/YY"  , 255 },
    { ITEM_TYPE_ACTION     ,  0,   0, "DATESCREEN", 255 }
  }
},

//Screen 12 - Brightness control
{
  3,
  {
    { ITEM_TYPE_HEAD       , 0 , 0  , "CPMDEADINT"    , 255 }, 
    { ITEM_TYPE_VARNUM     , 64, 64 , "BRIGHTNESS"    , 255 },
    { ITEM_TYPE_MENU_ACTION,  0,   7, "SaveBrightness", 2 }
  }
},

//Screen 13 - Warning configuration
{
  8,
  {
    { ITEM_TYPE_HEAD       , 0 , 0  , "CPMDEADINT" , 255}, 
    { ITEM_TYPE_VARNUM     , 10, 50 , "WARNCPM1"   , 255},
    { ITEM_TYPE_VARNUM     , 32, 50 , "WARNCPM2"   , 255},
    { ITEM_TYPE_VARNUM     , 54, 50 , "WARNCPM3"   , 255},
    { ITEM_TYPE_VARNUM     , 76, 50 , "WARNCPM4"   , 255},
    { ITEM_TYPE_VARNUM     , 98, 50 , "WARNCPM5"   , 255},
    { ITEM_TYPE_MENU_ACTION,  0,   7, "SaveWarnCPM", 2  },
    { ITEM_TYPE_LABEL      , 0 ,  20, "Warning CPM", 255}
  }
},
  
//Screen 14 - Language Selection Screen
{
  3,
  {
    { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT", 255},
    { ITEM_TYPE_MENU_ACTION, 1, 1, "English"   , 255},
    { ITEM_TYPE_MENU_ACTION, 2, 2, "Japanese"  , 3  }
  }
},

//Screen 15 - Total/Timer mode
{
  4,
  {
    { ITEM_TYPE_LABEL      ,255,32, "Average CPM", 255},
    { ITEM_TYPE_VARLABEL   ,255,48, "TTCOUNT"    , 255},
    { ITEM_TYPE_VARLABEL   ,255,90, "TTTIME"     , 255},
    { ITEM_TYPE_ACTION     ,  2, 2, "TOTALTIMER" , 255}
  }
},

//Screen 16 - User interface settings
{
  4,
  {
    { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT" , 255}, 
    { ITEM_TYPE_MENU       ,12, 1, "Brightness" , 10 },
    { ITEM_TYPE_MENU_ACTION, 0, 2, "Geiger Beep", 11 },
    { ITEM_TYPE_MENU       ,14, 3, "Language"   , 12 }
  }
},

//Screen 17 - Geiger settings
{
  4,
  {
    { ITEM_TYPE_HEAD       , 0, 0, "CPMDEADINT"    , 255}, 
    { ITEM_TYPE_MENU       , 5, 1, "Calibration"   , 255},
    { ITEM_TYPE_MENU_ACTION, 0, 2, "Clear Log"     , 255},
    { ITEM_TYPE_MENU       ,13, 3, "Warning Levels", 255}
//    { ITEM_TYPE_MENU,INVALID_SCREEN, 2, "Averaging Period" },
  }
},

//Screen 18 - Version information
{
  3,
  {
    { ITEM_TYPE_HEAD       , 0 , 0, "CPMDEADINT"        , 255}, 
    { ITEM_TYPE_LABEL      , 0  ,32, "Firmware Release" , 255},
    { ITEM_TYPE_LABEL      , 255,64, OS100VERSION       , 255}
  }
},

};

