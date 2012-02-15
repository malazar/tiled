#include "objectinspectordock.h"
#include "ui_objectinspectordock.h"
#include "mapdocument.h"
#include "preferences.h"
#include "propertiesmodel.h"
#include "changemapobject.h"
#include "resizemapobject.h"
#include "movemapobject.h"
#include "changeproperties.h"
#include <QDebug>
#include <QShortcut>

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

    //There should be a tooltip or something similar to inform
    //the user of the delete shortcut

    QShortcut *deleteShortcut = new QShortcut(Qt::Key_Backspace, ui->properties);


    connect(deleteShortcut, SIGNAL(activated()),
            this, SLOT(deleteSelectedProperties()));


    connect(ui->type->lineEdit(),SIGNAL(editingFinished()),
            this,SLOT(type_editingFinished()));

    connect(mPropertiesModel,SIGNAL(dataChanged(QModelIndex,QModelIndex))
            ,this,SLOT(dataChanged(QModelIndex,QModelIndex)));
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
    ui->height->clear();
    ui->properties->setModel(NULL);
}


void ObjectInspectorDock::setMapObject(MapObject * mapObject)
{
    MapObject * oldMapObject = mMapObject;

    mMapObject = mapObject;
    if (mMapObject)
    {
        mPropertiesModel->setProperties(mMapObject->properties());
        mObjectTypesModel->setObjectTypes(Preferences::instance()->objectTypes());
        ui->type->setModel(mObjectTypesModel);
        ui->name->setText(mMapObject->name());
        ui->type->setEditText(mMapObject->type());
        ui->x->setValue(mMapObject->x());
        ui->y->setValue(mMapObject->y());
        ui->width->setValue(mMapObject->width());
        ui->height->setValue(mMapObject->height());
        ui->properties->setModel(mPropertiesModel);

        ui->objectInspector->setEnabled(true);

        if (mMapObject != oldMapObject)
        {
            ui->name->setFocus();
        }

    }
    else
    {
        this->clear();
        ui->objectInspector->setEnabled(false);
    }

}


void ObjectInspectorDock::changeMapObject()
{
    QString newName = ui->name->text();
    QString newType = ui->type->currentText();
    QUndoStack *undo = mMapDocument->undoStack();

    undo->beginMacro(tr("Change Object"));
    undo->push(new ChangeMapObject(mMapDocument, mMapObject,
                                   newName, newType));
    undo->endMacro();
}

void ObjectInspectorDock::moveMapObject()
{
    qreal newX = ui->x->value();
    qreal newY = ui->y->value();

    QUndoStack *undo = mMapDocument->undoStack();
    const QPointF oldPos = mMapObject->position();

    mMapObject->setX(newX);
    mMapObject->setY(newY);

    undo->beginMacro(tr("Move Object"));
    undo->push(new MoveMapObject(mMapDocument, mMapObject, oldPos));
    undo->endMacro();
}


void ObjectInspectorDock::resizeMapObject()
{

    qreal newWidth = ui->width->value();
    qreal newHeight = ui->height->value();
    QUndoStack *undo = mMapDocument->undoStack();
    const QSizeF oldSize = mMapObject->size();

    mMapObject->setWidth(newWidth);
    mMapObject->setHeight(newHeight);

    undo->beginMacro(tr("Resize Object"));
    undo->push(new ResizeMapObject(mMapDocument, mMapObject, oldSize));
    undo->endMacro();

}

void ObjectInspectorDock::changeProperties()
{
    QUndoStack *undo = mMapDocument->undoStack();
    QString kind = tr("Object");

    undo->beginMacro(tr("Change Object Properties"));
    undo->push(new ChangeProperties(kind,mMapObject,mPropertiesModel->properties()));
    undo->endMacro();
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
        connect(mMapDocument,SIGNAL(currentLayerIndexChanged(int)),
                this,SLOT(layerChanged(int)));
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
    if(objects.count() == 1)
    {
        setMapObject(objects.first());
    }
    else
    {
        mMapObject = NULL;
        clear();
    }
}

void ObjectInspectorDock::on_name_editingFinished()
{
    QString newName = ui->name->text();
    if (ui->objectInspector->isEnabled() && mMapObject->name()!=newName)
    {
        changeMapObject();
    }
}

void ObjectInspectorDock::type_editingFinished()
{
    QString newType= ui->type->lineEdit()->text();

    if (ui->objectInspector->isEnabled() && mMapObject->type()!=newType)
    {
        changeMapObject();
    }
}

void ObjectInspectorDock::on_x_editingFinished()
{
    qreal newValue = ui->x->value();
    if (ui->objectInspector->isEnabled() && mMapObject->x()!=newValue)
    {
        moveMapObject();
    }
}

void ObjectInspectorDock::on_y_editingFinished()
{
    qreal newValue = ui->y->value();
    if (ui->objectInspector->isEnabled() && mMapObject->y()!=newValue)
    {
        moveMapObject();
    }
}


void ObjectInspectorDock::on_width_editingFinished()
{
    qreal newValue = ui->width->value();
    if (ui->objectInspector->isEnabled() && mMapObject->width()!=newValue)
    {
        resizeMapObject();
    }
}


void ObjectInspectorDock::on_height_editingFinished()
{
    qreal newValue = ui->height->value();
    if (ui->objectInspector->isEnabled() && mMapObject->height()!=newValue)
    {
        resizeMapObject();
    }
}

void ObjectInspectorDock::dataChanged(QModelIndex tl, QModelIndex br)
{
    if (mMapObject->properties() != mPropertiesModel->properties())
    {
        changeProperties();
    }
}

void ObjectInspectorDock::deleteSelectedProperties()
{
    //Borrowed from properties dialog
    QItemSelectionModel *selection = ui->properties->selectionModel();
    const QModelIndexList indices = selection->selectedRows();
    if (!indices.isEmpty())
    {
        mPropertiesModel->deleteProperties(indices);
        selection->select(ui->properties->currentIndex(),
                          QItemSelectionModel::ClearAndSelect |
                          QItemSelectionModel::Rows);
    }

    changeProperties();

}

void ObjectInspectorDock::layerChanged(int index)
{
    //Layer is changed, lets get ready to fetch a new object;
    setMapObject(NULL);
}
