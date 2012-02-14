#ifndef OBJECTINSPECTORDOCK_H
#define OBJECTINSPECTORDOCK_H

#include "mapdocument.h"
#include "mapobject.h"
#include "objecttypesmodel.h"

#include <QDockWidget>

namespace Ui {
    class ObjectInspectorDock;
}

namespace Tiled
{
namespace Internal
{
class PropertiesModel;


class ObjectInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit ObjectInspectorDock(QWidget *parent = 0);
    ~ObjectInspectorDock();
    void setMapDocument(MapDocument *);

private:
    void setMapObject(MapObject *);
    void clear();

private:
    Ui::ObjectInspectorDock *ui;
    MapDocument *mMapDocument;
    MapObject *mMapObject;
    ObjectTypesModel *mObjectTypesModel;
    PropertiesModel *mPropertiesModel;

private slots:
    void objectsChanged(const QList<MapObject*> &objects);
    void selectedObjectsChanged();
    void on_name_editingFinished();
    void type_editingFinished();
    void on_type_currentIndexChanged(QString);
};

}//namespace Internal
}//namespace Tiled

#endif // OBJECTINSPECTORDOCK_H
