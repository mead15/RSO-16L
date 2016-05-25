#ifndef EXECUTER_H
#define EXECUTER_H


class Executer : public QThread
{
public:
    Executer();

protected:
    void run();
};



#endif // EXECUTER_H
