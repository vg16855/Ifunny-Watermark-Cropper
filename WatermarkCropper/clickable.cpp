#include "clickable.h"

void Clickable::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
