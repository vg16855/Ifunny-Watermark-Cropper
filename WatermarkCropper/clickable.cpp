#include "clickable.h"

void Clickable::mousePressEvent(QMouseEvent*)
{
    emit clicked();
}
