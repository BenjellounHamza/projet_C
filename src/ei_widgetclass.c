#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ei_widget.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"

/*une liste chainee qui contient les differents widgetclass*/
extern ei_widgetclass_t* linked_widgetclasses;


/* cherche si widgetclass est deja dans la liste list_widget_class, si elle existe deja elle ne fait rien
    sinon elle rajoute la nouvelle classe a la queue de cette liste */
void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
    /* pour ne pas perdre la list_widget_class qui pointe vers la premiere classe
     * de la bibliotheque on va introduire un nouveau pointeur intermediaire*/
    ei_widgetclass_t *current = linked_widgetclasses;

    if (current == NULL)
        linked_widgetclasses = widgetclass;
    else
    {
        ei_widgetclass_t *previous = current;
        while (current != NULL)
        {
            if (!strcmp(ei_widgetclass_stringname(current->name),
                       ei_widgetclass_stringname(widgetclass->name)))
            {
                printf("the widgetclass already exist !\n");
                break;
            }
            previous = current;
            current = current->next;
        }
        if (current == NULL)
            previous->next = widgetclass;
    }
}


void ei_frame_register_class ()
{
    /* creer les fonctions propres a la classe frame */
    ei_widgetclass_t *ei_frame_class = malloc(sizeof(ei_widgetclass_t));

    strcpy(ei_frame_class->name, "frame");
    ei_frame_class->allocfunc = &ei_frame_allocfunc;
    ei_frame_class->releasefunc = &ei_frame_releasefunc;
    ei_frame_class->drawfunc = &ei_frame_drawfunc;
    ei_frame_class->setdefaultsfunc = &ei_frame_setdefaultsfunc;
    ei_frame_class->geomnotifyfunc = &ei_frame_geomnotifyfunc;
    ei_frame_class->handlefunc= &ei_frame_handlefunc;
    ei_frame_class->next = NULL;

    ei_widgetclass_register(ei_frame_class);
}


void ei_button_register_class ()
{
    /* creer les fonctions propres a la classe button */
    ei_widgetclass_t *ei_button_class = malloc(sizeof(ei_widgetclass_t));

    strcpy(ei_button_class->name, "button");
    ei_button_class->allocfunc = &ei_button_allocfunc;
    ei_button_class->releasefunc = &ei_button_releasefunc;
    ei_button_class->drawfunc = &ei_button_drawfunc;
    ei_button_class->setdefaultsfunc = &ei_button_setdefaultsfunc;
    ei_button_class->geomnotifyfunc = &ei_button_geomnotifyfunc;
    ei_button_class->handlefunc= &ei_button_handlefunc;
    ei_button_class->next = NULL;

    ei_widgetclass_register(ei_button_class);
}


void ei_toplevel_register_class ()
{
    ei_widgetclass_t *ei_toplevel_class = malloc(sizeof(ei_widgetclass_t));

    strcpy(ei_toplevel_class->name, "toplevel");
    ei_toplevel_class->allocfunc = &ei_toplevel_allocfunc;
    ei_toplevel_class->releasefunc = &ei_toplevel_releasefunc;
    ei_toplevel_class->drawfunc = &ei_toplevel_drawfunc;
    ei_toplevel_class->setdefaultsfunc = &ei_toplevel_setdefaultsfunc;
    ei_toplevel_class->geomnotifyfunc = &ei_toplevel_geomnotifyfunc;
    ei_toplevel_class->handlefunc= &ei_toplevel_handlefunc;
    ei_toplevel_class->next = NULL;

    ei_widgetclass_register(ei_toplevel_class);
}
