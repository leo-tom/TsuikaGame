

#include "Tsuika.h"
#include <gtk/gtk.h>

typedef enum{
    Cherry,
    Strawberry,
    Grape,
    Onion,
    Kaki,
    Apple,
    Pear,
    Peach,
    Pinapple,
    Melon,
    Suika
}FruitType;

double FruitType2Radius(FruitType type){
    switch(type){
        case Cherry:
            return 3;
        case Strawberry:
            return 5;
        case Grape:
            return 8;
        case Onion:
            return 15;
        case Kaki:
            return 25;
        case Apple:
            return 40;
        case Pear:
            return 50;
        case Peach:
            return 65;
        case Pinapple:
            return 80;
        case Melon:
            return 100;
        case Suika:
            return 150;
    }
}

double FruitType2Mass(FruitType type){
    switch(type){
        case Cherry:
            return 1;
        case Strawberry:
            return 2;
        case Grape:
            return 3;
        case Onion:
            return 4;
        case Kaki:
            return 5;
        case Apple:
            return 6;
        case Pear:
            return 7;
        case Peach:
            return 8;
        case Pinapple:
            return 9;
        case Melon:
            return 10;
        case Suika:
            return 11;
    }
}

double FruitType2RGB(FruitType type){
    switch(type){
        case Cherry:
            return 0xFF3333;
        case Strawberry:
            return 0xCC0000;
        case Grape:
            return 0x9933FF;
        case Onion:
            return 0xFFA64D;
        case Kaki:
            return 0xFF7733;
        case Apple:
            return 0xFF0000;
        case Pear:
            return 0xFFCC66;
        case Peach:
            return 0xFF6680;
        case Pinapple:
            return 0xFFFF00;
        case Melon:
            return 0x66FF19;
        case Suika:
            return 0x008015;
    }
}

typedef struct{
    FruitType type;
    char state; // 0 for inactive, 1 for active, 2 for grabbed
    double x;
    double y;
    double vx;
    double vy;
}Fruit;


Fruit fruits[MAX_FRUITS];
Fruit *grabbedFruit = NULL;
Fruit *nextFruit = NULL;
Fruit * getNewFruit(){
    for(int i = 0;i < MAX_FRUITS;i++){
        if(fruits[i].state == 0){
            return &fruits[i];
        }
    }
}
Fruit * getNextFruit(){
    Fruit *fruit = getNewFruit();
    fruit->type = rand() % 5;
    fruit->state = 2;
    fruit->y = 15;
}
void initFruits(){
    for(int i = 0;i < MAX_FRUITS;i++){
        fruits[i].state = 0;
        fruits[i].x = -100;
        fruits[i].y = -100;
        fruits[i].vx = 0;
        fruits[i].vy = 0;
    }
    grabbedFruit = getNextFruit();
    nextFruit    = getNextFruit();
}


void freeFruit(Fruit *fruit){
    fruit->state = 0;
    fruit->x = -100;
    fruit->y = -100;
    fruit->vx = 0;
    fruit->vy = 0;
}


GtkWidget *REF_TO_DRAWING_AREA = NULL;
double     mouse_x_coord       = 0;

void draw(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data){
    //fill background with white
    cairo_set_source_rgb(cr, 1,1,1);
    cairo_rectangle(cr,0,0,AREA_WIDTH,AREA_HEIGHT);
    cairo_fill(cr);
    //draw fruits
    for(int i = 0;i < MAX_FRUITS;i++){
        if(fruits[i].state == 1){
            int rgb = FruitType2RGB(fruits[i].type);
            int r = rgb >> 16;
            int g = (rgb >> 8) & 0xFF;
            int b = rgb & 0xFF;
            cairo_set_source_rgb(cr, r/255.0 ,g/255.0, b/255.0);
            cairo_arc(cr, fruits[i].x, fruits[i].y, FruitType2Radius(fruits[i].type), 0, 2*G_PI);
            cairo_fill(cr);
        }
    }
    //draw grabed fruit
    {
        int rgb = FruitType2RGB(grabbedFruit->type);
        int r = rgb >> 16;
        int g = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;
        cairo_set_source_rgb(cr, r/255.0 ,g/255.0, b/255.0);
        cairo_arc(cr, mouse_x_coord, 15, FruitType2Radius(grabbedFruit->type), 0, 2*G_PI);
        cairo_fill(cr);
    }
}

void mouse_moved(GtkEventControllerMotion *controller,double new_x,double new_y,gpointer user_data){
    static int x = 0;
    if(x != new_x){
        x = new_x;
        mouse_x_coord = x;
        grabbedFruit->x = mouse_x_coord;
    }
}
void mouse_clicked (GtkGestureClick *gesture,int n_press,double x,double y,GtkWidget *widget){
    grabbedFruit->state = 1;
    grabbedFruit->vy    = 10;
    grabbedFruit->x = mouse_x_coord;
    grabbedFruit = nextFruit;
    nextFruit = getNextFruit();
}

double distance(Fruit *fruit1,Fruit *fruit2){
    double dx = fruit1->x - fruit2->x;
    double dy = fruit1->y - fruit2->y;
    return sqrt(dx*dx + dy*dy);
}

gboolean physics(GtkWidget *area){
    const double time_diff_in_sec = TIME_DIFF / 1000.0;
    size_t SIZE = 0;
    Fruit *active_fruits[MAX_FRUITS];
    //gravity
    for(int i = 0;i < MAX_FRUITS;i++){
        if(fruits[i].state == 1){
            active_fruits[SIZE++] = &fruits[i];
        }
    }
    
    for(int i = 0;i < SIZE;i++){
        double new_x = active_fruits[i]->x + 20 * active_fruits[i]->vx * time_diff_in_sec;
        double new_y = active_fruits[i]->y + 20 * active_fruits[i]->vy * time_diff_in_sec;
        double new_vx = active_fruits[i]->vx;
        double new_vy = active_fruits[i]->vy;
        double radius = FruitType2Radius(active_fruits[i]->type);
        double HanpatsuKeisu = 0.8;


        if(new_x - radius <= 0.0){
            new_x = 0.0 + radius;
            new_vx = -HanpatsuKeisu*new_vx;
        }else if(new_x + radius >= AREA_WIDTH){
            new_x = AREA_WIDTH - radius;
            new_vx = -HanpatsuKeisu*new_vx;
        }
        if(new_y + radius >= AREA_HEIGHT){
            new_y = AREA_HEIGHT - radius;
            new_vy *= 0.05;
        }


        bool collisionOccured = false;
        for(int j = 0;j < SIZE;j++){
            if(i == j){
                continue;
            }
            double xdiff = new_x - active_fruits[j]->x;
            double ydiff = new_y - active_fruits[j]->y;
            double dist = sqrt(xdiff*xdiff + ydiff * ydiff);
            double radiusI = FruitType2Radius(active_fruits[i]->type);
            double radiusJ = FruitType2Radius(active_fruits[j]->type);
            if(dist < radiusI + radiusJ ){
                // Fruits are connected.
                if(active_fruits[i]->type == active_fruits[j]->type){
                    //upgrade fruit
                    new_x = (active_fruits[i]->x + active_fruits[j]->x) / 2.0;
                    new_y = (active_fruits[i]->y + active_fruits[j]->y) / 2.0;
                    active_fruits[i]->type++;
                    new_vx = 0;
                    new_vy = 10;
                    freeFruit(active_fruits[j]);
                    break;
                }else{
                    //norm of these numbers should be close to one.
                    collisionOccured = true;
                    double vec_x = (new_x - active_fruits[j]->x) / (radiusI + radiusJ);
                    double vec_y = (new_y - active_fruits[j]->y) / (radiusI + radiusJ);
                    double massI = FruitType2Mass(active_fruits[i]->type);
                    double massJ = FruitType2Mass(active_fruits[j]->type);
                    new_vx += 0.5*(massI/(massI + massJ) ) *vec_x;
                    new_vy += 0.5*(massI/(massI + massJ) ) *vec_y;
                    active_fruits[j]->vx -= 0.5*(massJ/(massI + massJ) ) *vec_x;
                    active_fruits[j]->vy -= 0.5*(massJ/(massI + massJ) ) *vec_y;
                }
            }
        }
        if(collisionOccured){
            //active_fruits[i]->x =  ( new_x + active_fruits[i]->x ) / 2.0;
            //active_fruits[i]->y = ( new_y + active_fruits[i]->y ) / 2.0;
        }else{
            active_fruits[i]->x = new_x;
            active_fruits[i]->y = new_y;
        }
        
        active_fruits[i]->vx = new_vx;
        active_fruits[i]->vy = new_vy;
    }

    for(int i = 0;i < SIZE;i++){
        if(active_fruits[i]->state == 1){
            active_fruits[i]->vx *= 0.95;
            if(active_fruits[i]->vy < 0){
                active_fruits[i]->vy *= 0.60;
            }else if(active_fruits[i]->vy >= 10){
                active_fruits[i]->vy = 10;
            }
            if(fabs(active_fruits[i]->vy) <= 0.1){
                active_fruits[i]->vy = 10;
            }
        }
    }
    gtk_widget_queue_draw(REF_TO_DRAWING_AREA);
    return true;
}

void initGraphics(GtkApplication* app, gpointer user_data){
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tsuika Game");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_widget_set_halign(frame, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(frame, GTK_POS_BOTTOM);
    gtk_window_set_child(GTK_WINDOW(window), frame);
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, AREA_WIDTH, AREA_HEIGHT);
    gtk_frame_set_child(GTK_FRAME(frame), drawing_area);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw, NULL, NULL);
    REF_TO_DRAWING_AREA = drawing_area;
    
    GtkEventController *controller = gtk_event_controller_motion_new();
    gtk_event_controller_set_propagation_phase (controller,GTK_PHASE_CAPTURE);
    g_signal_connect_swapped(controller, "motion", G_CALLBACK(mouse_moved), drawing_area);
    gtk_widget_add_controller(drawing_area, controller);

    //add mouse clicked event
    GtkGestureClick *gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),GDK_BUTTON_PRIMARY);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(gesture));
    g_signal_connect(gesture, "released", G_CALLBACK(mouse_clicked), drawing_area);

    g_timeout_add(TIME_DIFF,physics,drawing_area);

    gtk_window_present(window);
    //time_handler(window);
}



int main(){
    initFruits();
    GtkApplication *app = gtk_application_new("chibunny.is.cute", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(initGraphics), NULL);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    return status;    

}