#ifndef DALE_POSITION
#define DALE_POSITION

namespace dale
{
class Position
{
    /* for the time being */
public:
    int line_number;
    int column_number;
    Position();
    Position(int new_line_number, int new_column_number);
    Position(Position *pos);
    int getLineNumber(void);
    int getColumnNumber(void);
    void copyTo(Position *p);
    void setLineAndColumn(int new_line_number, int new_column_number);
};
}

#endif
