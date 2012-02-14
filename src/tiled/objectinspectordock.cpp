#include "objectinspectordock.h"
#include "ui_objectinspectordock.h"
#include "mapdocument.h"
#include "preferences.h"
#include "propertiesmodel.h"
#include <QDebug>

using namespace Tiled::Internal;

ObjectInspectorDock::ObjectInspectorDock(QWidget *parent) :
    QDockWidget(parent),
    mMapDocument(0),
    ui(new Ui::ObjectInspectorDock)
{
    ui->setupUi(this);

    mObjectTypesModel =  new ObjectTypesModel(this);
    mPropertiesModel = new PropertiesModel(this);
    ui->objectInspector->setEnabled(false);

}

ObjectInspectorDock::~ObjectInspectorDock()
{
    delete ui;
}


void ObjectInspectorDock::clear()
{
    ui->name->clear();
    ui->type->clearEditText();
    ui->x->clear();
    ui->y->clear();
    ui->width->clear();
    ui->heigth->clear();
    ui->properties->setModel(NULL);
}


void ObjectInspectorDock::setMapObject(MapObject * mapObject)
{

    mMapObject = mapObject;
    ui->objectInspector->setEnabled(false);
    if (mMapObject)
    {
        mPropertiesModel->setProperties(mMapObject->properties());
        mObjectTypesModel->setObjectTypes(Preferences::instance()->objectTypes());
        ui->type->setModel(mObjectTypesModel);
        mMapObject=mapObject;
        ui->name->setText(mMapObject->name());
        ui->type->setEditText(mMapObject->type());
        ui->x->setValue(mMapObject->x());
        ui->y->setValue(mMapObject->y());
        ui->width->setValue(mMapObject->width());
        ui->heigth->setValue(mMapObject->height());
        ui->properties->setModel(mPropertiesModel);
        ui->objectInspector->setEnabled(true);
    }
    else
    {
        this->clear();
    }

}

void ObjectInspectorDock::setMapDocument(MapDocument *mapDocument)
{
    if(mMapDocument==mapDocument)
    {
        return;
    }
    if(mMapDocument)
    {
        mMapDocument->disconnect(this);
    }

    if (mapDocument)
    {

        mMapDocument = mapDocument;

        connect(mMapDocument,SIGNAL(objectsChanged(const QList<MapObject*>)),
                this,SLOT(objectsChanged(QList<MapObject*>)));
        connect(mMapDocument,SIGNAL(selectedObjectsChanged()),
                this,SLOT(selectedObjectsChanged()));
    }

}

void ObjectInspectorDock::objectsChanged(const QList<MapObject *> &objects)
{
   QSet<MapObject *> set = objects.toSet();
   if (set.contains(mMapObject))
   {
       setMapObject(mMapObject);
   }
}

void ObjectInspectorDock::selectedObjectsChanged()
{
    ui->objectInspector->setEnabled(false);
    QList<MapObject *> objects = mMapDocument->selectedObjects();
    if(objects.count() > 0)
    {
        setMapObject(objects.first());
    }
    else
    {
        setMapObject(NULL);
    }
}

void ObjectInspectorDock::on_name_editingFinished()
{
    qDebug() << "Name changed";
}

void ObjectInspectorDock::on_type_currentIndexChanged(QString newType)
{
    if (ui->objectInspector->isEnabled() && mMapObject->type()!=newType)
    {
        qDebug() << newType;
    }
}

