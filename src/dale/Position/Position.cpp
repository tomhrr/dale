#include "Position.h"

namespace dale
{
Position::Position()
{
    line_number = 0;
    column_number = 0;
}

Position::Position(int new_line_number, int new_column_number)
{
    line_number   = new_line_number;
    column_number = new_column_number;
}

Position::Position(Position *pos)
{
    line_number   = pos->line_number;
    column_number = pos->column_number;
}

void Position::setLineAndColumn(int new_line_number, int new_column_number)
{
    line_number   = new_line_number;
    column_number = new_column_number;
}

int Position::getLineNumber(void)
{
    return line_number;
}

int Position::getColumnNumber(void)
{
    return column_number;
}

void Position::copyTo(Position *p)
{
    p->line_number   = line_number;
    p->column_number = column_number;
}
}
