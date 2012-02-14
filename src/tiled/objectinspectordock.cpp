#include "objectinspectordock.h"
#include "ui_objectinspectordock.h"

ObjectInspectorDock::ObjectInspectorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ObjectInspectorDock)
{
    ui->setupUi(this);
}

ObjectInspectorDock::~ObjectInspectorDock()
{
    delete ui;
}
