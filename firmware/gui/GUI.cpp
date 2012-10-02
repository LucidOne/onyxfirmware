#include "screen_layout.h"
#include "Controller.h"
#include <stdint.h>
#include "display.h"
#include "UserInput.h"
#include "GUI.h"
#include <stdint.h>
#include "utils.h"
#include "power.h"
#include "realtime.h"
#include <stdio.h>
#include "Geiger.h"

#define KEY_BACK   6
#define KEY_HOME   8
#define KEY_DOWN   4
#define KEY_UP     3
#define KEY_SELECT 2
#define KEY_HELP   0
#define KEY_PRESSED  0
#define KEY_RELEASED 1

bool first_render=true;

uint16 header_color;

uint8_t m_language;

void display_draw_equtriangle(uint8_t x,uint8_t y,uint8_t s,uint16_t color) {

  uint8_t start = x;
  uint8_t end   = x;
  for(uint8_t n=0;n<s;n++) {
 
    for(uint8_t i=start;i<=end;i++) {
      display_draw_point(i,y,color);
    }
    start--;
    end++;
    y++;
  }
}

void display_draw_equtriangle_inv(uint8_t x,uint8_t y,uint8_t s,uint16_t color) {

  uint8_t start = x;
  uint8_t end   = x;
  for(uint8_t n=0;n<s;n++) {
 
    for(uint8_t i=start;i<=end;i++) {
      display_draw_point(i,y,color);
    }
    start--;
    end++;
    y--;
  }
}

void render_item_menu(screen_item &item, bool selected) {

  uint16_t highlight = FOREGROUND_COLOR;
  if(selected) highlight = BACKGROUND_COLOR;

  if((m_language == LANGUAGE_ENGLISH) || (item.kanji_image == 255)) {

    int len = strlen(item.text);
    char text[50];
    strcpy(text,item.text);
    for(int n=len;n<16;n++) {
      text[n  ]=' ';
      text[n+1]=0;
    }

    display_draw_text(0,item.val2*16,text,highlight);
  } else
  if(m_language == LANGUAGE_JAPANESE) {
    display_draw_fixedimage(0,item.val2*16,item.kanji_image,highlight);
  }
}

#define VARNUM_MAXSIZE 10

char    varnum_names[VARNUM_MAXSIZE][10];
uint8_t varnum_values[VARNUM_MAXSIZE];
uint8_t varnum_size = 0;

uint8_t get_item_state_varnum(const char *name) {

  for(uint32_t n=0;n<varnum_size;n++) {
    if(strcmp(varnum_names[n],name) == true) {
      return varnum_values[n];
    }
  }
  return 0;
}

void set_item_state_varnum(char *name,uint8_t value) {
  for(uint32_t n=0;n<varnum_size;n++) {
    if(strcmp(varnum_names[n],name) == true) {
      varnum_values[n] = value;
      return;
    }
  }
  
  if(varnum_size >= VARNUM_MAXSIZE) return;
  strcpy(varnum_names[varnum_size],name);
  varnum_values[varnum_size] = value;
  varnum_size++;
}

void render_item_varnum(screen_item &item, bool selected) {

  uint8_t x = item.val1;
  uint8_t y = item.val2;

  uint8_t val = get_item_state_varnum(item.text);

  uint16_t color;
  if(selected) color = 0xcccc; else color = FOREGROUND_COLOR;
  display_draw_equtriangle(x,y,9,color);
  display_draw_equtriangle_inv(x,y+33,9,color);
  display_draw_number(x-4,y+9,val,1,FOREGROUND_COLOR);
}
      
uint8_t get_item_state_varnum(screen_item &item) {
  return get_item_state_varnum(item.text);
}

uint32_t delay_time;
uint32_t get_item_state_delay(screen_item &item) {
  return delay_time;
}

void clear_item_varnum(screen_item &item, bool selected) {
  int x = item.val1;
  int y = item.val2;
  int start_x = x-9;
  int end_x   = x+9;

  int start_y = y-2;
  int end_y   = y+40;

  if(start_x <   0) start_x = 0;
  if(  end_x > 127)   end_x = 127;

  if(start_y <   0) start_y = 0;
  if(  end_y > 127)   end_y = 127;
  
  display_draw_rectangle(start_x,start_y,end_x,end_y,BACKGROUND_COLOR);
}


void render_item_label(screen_item &item, bool selected) {
  if(item.val1 == 255) {display_draw_text_center          (item.val2,item.text,FOREGROUND_COLOR);}
                  else {display_draw_text       (item.val1,item.val2,item.text,FOREGROUND_COLOR);}
}

void render_item_smalllabel(screen_item &item, bool selected) {
  if(item.val1 == 255) {display_draw_tinytext_center          (item.val2,item.text,FOREGROUND_COLOR);}
                  else {display_draw_tinytext       (item.val1,item.val2,item.text,FOREGROUND_COLOR);}
}

void render_item_head(screen_item &item, bool selected) {
  draw_text(0,0,"                ",header_color);
}

float m_graph_data[240];
float *source_graph_data;
bool  graph_first;

void render_item_graph(screen_item &item, bool selected) {

  int32_t data_size=240;
  int32_t data_offset=600-240;
  int32_t data_increment=2;
  int32_t x_spacing=1;
  float   max_height = 80;

  int32_t m_x = item.val1;
  int32_t m_y = item.val2;
  
  graph_first = first_render;

  // find min and max in old data
  int32_t omax = m_graph_data[0];
  int32_t omin = m_graph_data[0];
  for(uint32_t n=0;n<(data_size/data_increment);n++) {
    if(m_graph_data[n+data_offset] > omax) omax = m_graph_data[n+data_offset];
    if(m_graph_data[n+data_offset] < omin) omin = m_graph_data[n+data_offset];
  }

  // find min and max in new data
  int32_t nmax = source_graph_data[data_offset];
  int32_t nmin = source_graph_data[data_offset];
  for(uint32_t n=0;n<data_size;n++) {
    if(source_graph_data[n+data_offset] > nmax) nmax = source_graph_data[n+data_offset];
    if(source_graph_data[n+data_offset] < nmin) nmin = source_graph_data[n+data_offset];
  }


  display_draw_line(m_x,m_y           ,m_x+(data_size/data_increment),m_y           ,FOREGROUND_COLOR);
  display_draw_line(m_x,m_y-max_height,m_x+(data_size/data_increment),m_y-max_height,FOREGROUND_COLOR);
  
  if((nmin == 0) && (nmax == 0)) {
    display_draw_tinynumber(m_x+5,m_y-max_height+10,nmax,4,FOREGROUND_COLOR);
    display_draw_tinynumber(m_x+5,m_y-10           ,nmin,4,FOREGROUND_COLOR);
    return;
  }

  int32_t lastx=m_x;
  int32_t lastoy=m_graph_data[0];
  int32_t lastny=m_y-((((float)source_graph_data[data_offset]-(float)nmin)/(float)(nmax-nmin))*max_height);
  int32_t cx = m_x+x_spacing;
  for(uint32_t n=0;n<data_size;n+=data_increment) {
    int oy = m_graph_data[n/data_increment];

    
    float source_data=0;
    for(uint32_t i=0;i<data_increment;i++) {
      float datapoint = source_graph_data[n+data_offset+i];
      source_data += datapoint;
    }
    source_data = source_data/data_increment;

    int ny = m_y-((((float)source_data-(float)nmin)/(float)(nmax-nmin))*max_height);
    if(!((lastoy == lastny) && (oy == ny) && !graph_first)) {
      if(!first_render) display_draw_line(lastx,lastoy,cx,oy,BACKGROUND_COLOR);
                        display_draw_line(lastx,lastny,cx,ny,FOREGROUND_COLOR);
    }
    m_graph_data[n/data_increment] = ny;
    lastx=cx;
    lastoy=oy;
    lastny=ny;
    cx+=x_spacing;
  }
  display_draw_tinynumber(m_x+5,m_y-max_height+10,nmax,4,FOREGROUND_COLOR);
  display_draw_tinynumber(m_x+5,m_y-10           ,nmin,4,FOREGROUND_COLOR);
}

void clear_item_menu(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;

  
  uint8_t x_max=127;
  uint8_t y_max=(item.val2+1)*16;
  if(y_max > 127) y_max=127;
  display_draw_rectangle(0,item.val2*16,x_max,y_max,BACKGROUND_COLOR);
}

void clear_item_label(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;
   
  int x_max = 0;
  if(item.val1 == 255) { x_max = 127;} else
                       { x_max = item.val1+(text_len*8)-1;}
  int y_max = item.val2+16;
  if(x_max>=128) x_max=127;
  if(y_max>=128) y_max=127;
  int x_min = 0;
  if(item.val1 != 255) {x_min = item.val1; x_max=127;}
  display_draw_rectangle(x_min,item.val2,x_max,y_max,BACKGROUND_COLOR);
}

void clear_item_smalllabel(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;
   
  int x_max = 0;
  if(item.val1 == 255) { x_max = 127;} else
                       { x_max = item.val1+(text_len*8)-1;}
  int y_max = item.val2+16;
  if(x_max>=128) x_max=127;
  if(y_max>=128) y_max=127;
  int x_min = 0;
  if(item.val1 != 255) {x_min = item.val1; x_max=127;}
  display_draw_rectangle(x_min,item.val2,x_max,y_max,BACKGROUND_COLOR);
}

void clear_item_head(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;

  int x_max=127;
  int y_max=16;
  display_draw_rectangle(item.val1,item.val2,x_max,y_max,BACKGROUND_COLOR);
}
    
void clear_item_bigvarlabel(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;
  display_draw_rectangle(item.val1,item.val2,127,(item.val2+32),BACKGROUND_COLOR);
}


void clear_item_varlabel(screen_item &item, bool selected) {
  int32_t text_len = strlen(item.text);

  if(text_len == 0) return;
  int x=0;
  if(item.val1 == 255) x = 0; else x = item.val1;
  display_draw_rectangle(x,item.val2,127,(item.val2+16),BACKGROUND_COLOR);
}

void clear_item_graph(screen_item &item, bool selected) {

  display_draw_rectangle(0,16,128,128,BACKGROUND_COLOR);
/*
  int32_t m_x = item.val1;
  int32_t m_y = item.val2;
  
  graph_first = true;
  int32_t size=60;

  int offset=60;
  int32_t lastx=m_x;
  int32_t lastoy=m_y-m_graph_data[offset];
  for(uint32_t n=0;n<size;n++) {
    int cx = m_x+n;
    int oy = m_y-m_graph_data[n+offset];
    display_draw_line(lastx,lastoy,cx,oy,BACKGROUND_COLOR);
    lastx=cx;
    lastoy=oy;
  }
*/
}

void clear_item_delay(screen_item &item, bool selected) {
  display_draw_rectangle(item.val1,item.val2,item.val1+24,item.val2+16,BACKGROUND_COLOR);
}

void render_item_delay(screen_item &item,bool selected) {
  display_draw_number(item.val1,item.val2,delay_time,3,FOREGROUND_COLOR);
}


void render_item(screen_item &item,bool selected) {
  if(item.type == ITEM_TYPE_MENU) {
    render_item_menu(item,selected);
  } else
  if(item.type == ITEM_TYPE_LABEL) {
    render_item_label(item,selected);
  } else
  if(item.type == ITEM_TYPE_SMALLLABEL) {
    render_item_smalllabel(item,selected);
  } else
  if(item.type == ITEM_TYPE_GRAPH) {
    render_item_graph(item,selected);
  } else 
  if(item.type == ITEM_TYPE_HEAD) {
    render_item_head(item,selected);
  } else 
  if(item.type == ITEM_TYPE_MENU_ACTION) {
    render_item_menu(item,selected);
  } else 
  if(item.type == ITEM_TYPE_VARNUM) {
    render_item_varnum(item,selected);
  } else 
  if(item.type == ITEM_TYPE_DELAY) {
    render_item_delay(item,selected);
  }
}

void clear_item(screen_item &item,bool selected) {
  if(item.type == ITEM_TYPE_MENU) {
    clear_item_menu(item,selected);
  } else
  if(item.type == ITEM_TYPE_LABEL) {
    clear_item_label(item,selected);
  } else
  if(item.type == ITEM_TYPE_SMALLLABEL) {
    clear_item_label(item,selected);
  } else
  if(item.type == ITEM_TYPE_VARLABEL) {
    clear_item_varlabel(item,selected);
  } else
  if(item.type == ITEM_TYPE_GRAPH) {
    clear_item_graph(item,selected);
  } else 
  if(item.type == ITEM_TYPE_HEAD) {
    clear_item_head(item,selected);
  } else
  if(item.type == ITEM_TYPE_MENU_ACTION) {
    clear_item_menu(item,selected);
  } else
  if(item.type == ITEM_TYPE_VARNUM) {
    clear_item_varnum(item,selected);
  } else
  if(item.type == ITEM_TYPE_DELAY) {
    clear_item_delay(item,selected);
  } else
  if(item.type == ITEM_TYPE_BIGVARLABEL) {
    clear_item_bigvarlabel(item,selected);
  }
}

void update_item_graph(screen_item &item,void *value) {
 source_graph_data = (float *)value;
}


uint8 lock_mask [11][8] = {
  
  {0,1,1,1,1,1,1,0},
  {1,1,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,0,0,1,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,1,0,0,1,1,1},
  {1,1,1,0,0,1,1,1},
  {1,1,1,0,0,1,1,1},
  {1,1,1,1,1,1,1,1}

};

void render_lock(bool on) {
  
  uint16 image_data[88]; // 8*11
  for(int x=0;x<8;x++) {
    for(int y=0;y<11;y++) {
      int16 render_value = lock_mask[y][x];

      if(render_value > 0)  render_value = 0xFFFF;
                       else render_value = 0;
 
      if(on == false) render_value = 0;

      image_data[(y*8)+x] = render_value;
    }
  }

  display_draw_image(128-8,128-11,8,11,image_data);
}

uint8 battery_mask [16][24] = {

  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0},
  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0},
  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0},
  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0},
  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0},
  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0},
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};

void render_battery(int x,int y,int level) {

  uint16 image_data[384]; // 24*16

  level = (((float) level)/100)*23;

  for(int x=0;x<24;x++) {
    for(int y=0;y<16;y++) {
      int16 render_value = battery_mask[y][x];

      if(x <= level) {
        if(render_value > 0)  render_value = 0xF1FF - (2081*(render_value-1));
                         else render_value = header_color;// HEADER_COLOR; // header background
      }

      if(x > level) {
        if(render_value > 0)  render_value = BACKGROUND_COLOR;  //6243 - (2081*(render_value-1));
                         else render_value = header_color;//HEADER_COLOR; // header background
      }
      image_data[(y*24)+x] = render_value;
    }
  }

  display_draw_image(104,0,24,16,image_data);
}

void update_item_head(screen_item &item,void *value) {

  uint16_t new_header_color;
  if(system_geiger->is_cpm_valid()) new_header_color = HEADER_COLOR_NORMAL;
                               else new_header_color = HEADER_COLOR_CPMINVALID;

  if(new_header_color != header_color) {
    draw_text(0,0,"                ",new_header_color);
  }
  header_color = new_header_color;

  int len = strlen((char *) value);
  char v[50];
  strcpy(v,(char *) value);
  for(int n=len;n<6;n++) {
    v[n  ] = ' ';
    v[n+1]=0;
  }
  if(len > 6) v[6]=0;

  draw_text(0,0,v,header_color);//HEADER_COLOR);

  // a hack!
  render_battery(0,128-24,power_battery_level());

  uint8_t hours,min,sec,day,month;
  uint16_t year;
  realtime_getdate(hours,min,sec,day,month,year);
  month+=1;
  year+=1900;
  if(year >= 2000) {year-=2000;} else
  if(year <  2000) {year-=1900;}

  char time[50];
  char date[50];
  sprintf(time,"%02u:%02u:%02u",hours,min,sec);
  sprintf(date,"%02u/%02u/%02u",month,day,year);
  if(year == 0) { sprintf(date,"%02u/%02u/00",month,day); }


  // pad out time and date
  int tlen = strlen(time);
  for(int n=tlen;n<8;n++) {
    time[n  ] = ' ';
    time[n+1]=0;
  }

  int dlen = strlen(date);
  for(int n=dlen;n<8;n++) {
    date[n  ] = ' ';
    date[n+1]=0;
  }

  display_draw_tinytext(128-75,2,time,header_color);//HEADER_COLOR);
  display_draw_tinytext(128-75,9,date,header_color);//HEADER_COLOR);
  display_draw_tinytext(0,128-5,OS100VERSION,FOREGROUND_COLOR);
}

void update_item_varnum(screen_item &item,void *value) {
  set_item_state_varnum(item.text,((uint8_t *) value)[0]);
}

void update_item_delay(screen_item &item,void *value) {

  if(first_render == true) {
    // parse out delay time
    delay_time = str_to_uint(item.text+8);

  }

  if(delay_time >= 1) delay_time--;
  delay_us(1000000);
  display_draw_number(item.val1,item.val2,delay_time,3,FOREGROUND_COLOR);
}

int get_item_state_delay_destination(screen_item &item) {
  // parse out destination screen

  for(int n=9;n<50;n++) {
    if(!((item.text[n] >= '0') && (item.text[n] <= '9'))) {
      uint32_t destination_screen_start = n+1;
      int dest = str_to_uint(item.text+destination_screen_start);
      return dest;
    }
  }
  return 0;
}

void update_item(screen_item &item,void *value) {
  if(item.type == ITEM_TYPE_VARLABEL) {
    if(item.val1 == 255) {
      // display_draw_rectangle(0,item.val2,128,item.val2+16,BACKGROUND_COLOR); unfortunately renders badly.
      display_draw_text_center(item.val2,(char *)value,FOREGROUND_COLOR);
    } else {
      display_draw_text(item.val1,item.val2,(char *)value,FOREGROUND_COLOR);
    }

  } else 
  if(item.type == ITEM_TYPE_GRAPH) {
    update_item_graph(item,value);
  } else 
  if(item.type == ITEM_TYPE_HEAD) {
    update_item_head(item,value);
  } else 
  if(item.type == ITEM_TYPE_VARNUM) {
    update_item_varnum(item,value);
  } else 
  if(item.type == ITEM_TYPE_DELAY) {
    update_item_delay(item,value);
  } else
  if(item.type == ITEM_TYPE_BIGVARLABEL) {
    display_draw_bigtext(item.val1,item.val2,(char *)value,FOREGROUND_COLOR);
  }
}
  
void GUI::clear_stack() {
  selected_stack_size=0;
}
  
void GUI::pop_stack(int &current_screen,int &selected_item) {

  if(selected_stack_size == 0) return;

  current_screen = selected_screen_stack[selected_stack_size-1];
  selected_item  = selected_item_stack[selected_stack_size-1];
  selected_stack_size--;
}

void GUI::push_stack(int current_screen,int selected_item) {

  if(selected_stack_size >= MAX_SCREEN_STACK) return;  

  selected_screen_stack[selected_stack_size] = current_screen;
  selected_item_stack  [selected_stack_size] = selected_item;
  selected_stack_size++;
}

GUI::GUI(Controller &r) : receive_gui_events(r) {

  new_keys_size = 0;
  clear_next_render=false;
  current_screen = 0;
  selected_item=1;
  clear_stack();
  m_trigger_any_key=false;
  m_sleeping=false;
  m_redraw=false;
  m_screen_lock=false;
  m_language = LANGUAGE_ENGLISH;
}


void GUI::render() {

  if(m_sleeping) {
     process_keys();
     return;  
  }

  // following two items really need to be atomic...
  int32_t cscreen = current_screen;
  int32_t citem   = selected_item;

  if(clear_next_render) {
    clear_next_render=false;
    clear_screen(clear_screen_screen,clear_screen_selected);
    first_render=true;
  }

  bool do_redraw = false;
  if(m_redraw) do_redraw = true;
  m_redraw = false;

  render_lock(m_screen_lock);
  for(uint32_t n=0;n<screens_layout[cscreen].item_count;n++) {

    if(first_render) {
      if(screens_layout[current_screen].items[n].type == ITEM_TYPE_ACTION) {
        receive_gui_events.receive_gui_event(screens_layout[cscreen].items[n].text,
                                             screens_layout[cscreen].items[n].text);
      }
    }

    bool selected = false;
    bool near_selected = false;
    if(n == citem) selected = true;

    if(n-1 == citem) near_selected = true;
    if(n+1 == citem) near_selected = true;

    if(n==0) near_selected = false;

    if(first_render || (selected) || (near_selected) || do_redraw) {
      render_item(screens_layout[cscreen].items[n],selected);
    }
  }
  first_render=false;
  process_keys();
}

void GUI::clear_screen(int32_t c_screen,int32_t c_selected) {
  for(uint32_t n=0;n<screens_layout[c_screen].item_count;n++) {

    bool selected = false;
    if(n == c_selected) selected = true;

    clear_item(screens_layout[c_screen].items[n],selected);
  }
  varnum_size=0;
}

void GUI::set_key_trigger() {

  m_trigger_any_key=true;

}

void GUI::redraw() {
  m_redraw = true;
}

void GUI::receive_key(int key_id,int type) {

  if(new_keys_size > NEW_KEYS_MAX_SIZE) return;

  new_keys_key [new_keys_size] = key_id;
  new_keys_type[new_keys_size] = type;
  new_keys_size++;
}

void GUI::process_keys() {

  //TODO: if a key press happens while we are in this loop, it will be lost
  for(uint32_t n=0;n<new_keys_size;n++) {
    process_key(new_keys_key[n],new_keys_type[n]);
  }
  new_keys_size=0;
}

void GUI::process_key(int key_id,int type) {

  if(m_screen_lock) return;

  if(m_trigger_any_key) {
    receive_gui_events.receive_gui_event("KEYPRESS","any");
    m_trigger_any_key=false;
  }

  if(m_sleeping) return;

  if((key_id == KEY_DOWN) && (type == KEY_RELEASED)) {

    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_VARNUM) {
      uint8_t current = get_item_state_varnum(screens_layout[current_screen].items[selected_item]);

      int8_t val[1];
      val[0] = current-1;
      if(val[0] < 0) val[0] = 0;
      update_item(screens_layout[current_screen].items[selected_item],val);
  
      receive_gui_events.receive_gui_event("varnumchange",screens_layout[current_screen].items[selected_item].text);
      return;
    }

    selected_item++;
    if(selected_item >= screens_layout[current_screen].item_count) {
      selected_item = screens_layout[current_screen].item_count-1;
    }
  }

  if((key_id == KEY_UP) && (type == KEY_RELEASED)) {

    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_VARNUM) {
      uint8_t current = get_item_state_varnum(screens_layout[current_screen].items[selected_item]);

      int8_t val[1];
      val[0] = current+1;
      if(val[0] > 9) val[0] = 9;
      update_item(screens_layout[current_screen].items[selected_item],val);
      receive_gui_events.receive_gui_event("varnumchange",screens_layout[current_screen].items[selected_item].text);
      return;
    }


    if(selected_item == 1) return;
    selected_item--;
  }

  if((key_id == KEY_SELECT) && (type == KEY_RELEASED)) {

    // if a VARNUM is selected...
    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_VARNUM) {
      if(selected_item != 0) {
        if((selected_item+1) < screens_layout[current_screen].item_count) {
          selected_item++;
          return;
        }
      }
    }

    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_MENU) {
      if(screens_layout[current_screen].items[selected_item].val1 != INVALID_SCREEN) {
        
        if(clear_next_render == false) {
          clear_next_render = true;
          first_render=true;
          clear_screen_screen   = current_screen;
          clear_screen_selected = selected_item;
        }

        push_stack(current_screen,selected_item);
	current_screen = screens_layout[current_screen].items[selected_item].val1;
	selected_item = 1;
      }
    } else 
    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_MENU_ACTION) {
      receive_gui_events.receive_gui_event(screens_layout[current_screen].items[selected_item].text,"select");
    }
    
  }

  if((key_id == KEY_BACK) && (type == KEY_RELEASED)) {

    // if a varnum is selected
    if(screens_layout[current_screen].items[selected_item].type == ITEM_TYPE_VARNUM) {
      if(selected_item != 0) {
        if((selected_item-1) >= 0) {
          if(screens_layout[current_screen].items[selected_item-1].type == ITEM_TYPE_VARNUM) {
            selected_item--;
            receive_gui_events.receive_gui_event("varnumchange",screens_layout[current_screen].items[selected_item].text);
            return;
          }
        }
      }
    }

    if(selected_stack_size !=0) {
      clear_next_render = true; 
      first_render=true;
      clear_screen_screen   = current_screen;
      clear_screen_selected = selected_item;

      pop_stack(current_screen,selected_item);
    }
  }

  if((key_id == KEY_HOME) && (type == KEY_RELEASED)) {

    if(current_screen != 0) {
      clear_next_render = true;
      first_render=true;
      clear_screen_screen   = current_screen;
      clear_screen_selected = selected_item;
      clear_stack();
      current_screen = 0;
      selected_item  = 1;
    }

  }
  
}

void GUI::jump_to_screen(const char screen) {
  clear_next_render = true;
  first_render = true;
  clear_screen_screen   = current_screen;
  clear_screen_selected = selected_item;

  clear_stack();
  current_screen = screen; 
  selected_item  = 1;
}

void GUI::receive_update(const char *tag,void *value) {
  for(uint32_t n=0;n<screens_layout[current_screen].item_count;n++) {
    if(strcmp(tag,screens_layout[current_screen].items[n].text) == true) {
      update_item(screens_layout[current_screen].items[n],value);

      // has to be in the GUI object, because we don't have access to current_screen outside it.
      if(screens_layout[current_screen].items[n].type == ITEM_TYPE_DELAY) {
        if(get_item_state_delay(screens_layout[current_screen].items[n]) == 0) {
          jump_to_screen(get_item_state_delay_destination(screens_layout[current_screen].items[n]));
        }
      }
    }
  }
}

void GUI::set_sleeping(bool v) {
  m_sleeping = v;
}

void GUI::toggle_screen_lock() {

  if(m_screen_lock == true) m_screen_lock = false; else m_screen_lock = true;
}

uint8_t GUI::get_item_state_uint8(const char *tag) {
  // should check item type and repsond appropriately, however only varnum currently returns uint8_t
  return get_item_state_varnum(tag);
}

void GUI::set_language(uint8_t lang) {
  m_language = lang;
}
