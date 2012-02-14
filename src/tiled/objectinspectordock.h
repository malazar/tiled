#ifndef OBJECTINSPECTORDOCK_H
#define OBJECTINSPECTORDOCK_H

#include <QDockWidget>

namespace Ui {
    class ObjectInspectorDock;
}

class ObjectInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit ObjectInspectorDock(QWidget *parent = 0);
    ~ObjectInspectorDock();

private:
    Ui::ObjectInspectorDock *ui;
};

#endif // OBJECTINSPECTORDOCK_H
