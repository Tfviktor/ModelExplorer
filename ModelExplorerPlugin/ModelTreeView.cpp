//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "ModelTreeView.h"
#include "ModelTreeBuilder.h"

#include <QtWidgets/QHeaderView.h>

#include <RengaAPI/ObjectIdCollection.h>
#include <RengaAPI/ObjectVisibility.h>

ModelTreeView::ModelTreeView(QTranslator* pTranslator, QWidget* pParent /*= nullptr*/)
  : QTreeView(pParent)
  , m_pTranslator(pTranslator)
{
  connect(&m_objectSelectionHandler, 
    SIGNAL(objectSelected(const rengaapi::ObjectId&)), 
    SLOT(onRengaObjectSelected(const rengaapi::ObjectId&)));

  connect(this, 
    SIGNAL(clicked(const QModelIndex&)), 
    this, 
    SLOT(onTreeItemClicked(const QModelIndex&)));
}

ModelTreeView::~ModelTreeView()
{}

void ModelTreeView::onRebuildTree()
{
  ModelTreeBuilder builder(m_pTranslator);

  m_pModel.reset(builder.buildModelTree());
  setModel(m_pModel.get());

  // set columns width & resize mode
  QHeaderView* treeHeader  = this->header();
  treeHeader->setStretchLastSection(false);
  treeHeader->setSectionResizeMode(treeHeader->logicalIndex(0), QHeaderView::ResizeMode::Stretch);
  treeHeader->setSectionResizeMode(treeHeader->logicalIndex(1), QHeaderView::ResizeMode::Fixed);
  treeHeader->resizeSection(1, 18);

  // check this
  QItemSelectionModel* pSelectionModel = selectionModel();
  connect(pSelectionModel, 
    SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
    SLOT(onTreeItemSelected(const QItemSelection&, const QItemSelection&)));


  // select first element in tree after rebuild
  pSelectionModel->select(m_pModel->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void ModelTreeView::onTreeItemSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  rengaapi::ObjectId selectedObjectId(0);
  if (!selected.empty())
  {
    QModelIndexList indexList = selected.indexes();
    if (!indexList.empty())
    {
      // get selected object id from tree view
      QModelIndex selectedObjectIndex = indexList.at(0);
      QVariant data = m_pModel->data(selectedObjectIndex, ModelTreeBuilder::objectIDRole);
      if (data.type() != QVariant::Invalid)
      {
        bool ok = false;
        unsigned int id = data.toUInt(&ok);
        if (ok)
        {
          selectedObjectId.setId(id);
          // update visibility only for rengaapi::ModelObjects
          const QModelIndex selectedIconIndex = m_pModel->index(selectedObjectIndex.row(), 1, selectedObjectIndex.parent());
          updateVisibilityIcon(selectedObjectIndex, selectedIconIndex);
        }
      }
    }
  }
  emit modelObjectSelectionChanged(selectedObjectId);
}

void ModelTreeView::onRengaObjectSelected(const rengaapi::ObjectId& objectId)
{
  QAbstractItemModel* pModel = model();
  assert(pModel != nullptr);

  QModelIndexList indexList = pModel->match(pModel->index(0,0), 
    ModelTreeBuilder::objectIDRole, 
    objectId.id(), 
    1,
    Qt::MatchRecursive);

  if (!indexList.empty())
  {
    QItemSelectionModel* pSelectionModel = selectionModel();
    pSelectionModel->setCurrentIndex(indexList.first(), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    pSelectionModel->select(indexList.first(), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

    expand(indexList.first());
  }
}

void ModelTreeView::showSelectedItem()
{
  changeItemVisibility(true);
}

void ModelTreeView::hideSelectedItem()
{
  changeItemVisibility(false);
}

void ModelTreeView::onTreeItemClicked(const QModelIndex& iconIndex)
{
  if (iconIndex.column() == 0)
    return;

  const QModelIndex itemIndex = m_pModel->index(iconIndex.row(), 0, iconIndex.parent());
  QStandardItem* item = m_pModel->itemFromIndex(itemIndex);
  QStandardItem* iconItem = m_pModel->itemFromIndex(iconIndex);

  bool isVisible;
  QVariant data = item->data();
  if (data.type() == QVariant::Invalid)
  {
    // get folder visibility
    updateVisibilityIcon(itemIndex, iconIndex);
    isVisible = iconItem->data().toBool();
    isVisible ^= true;
  }
  else
  {
    isVisible = isModelObjectVisible(data);
    isVisible ^= true;
  }

  // show parent items only if current item visible
  // Note: when you show level, all objects on level will be shown. It's renga bug, this behaviour will be changed later
  if (isVisible)
    showParentItems(item);

  // hide/show all children
  changeChildrenVisibility(iconIndex, isVisible);
}

void ModelTreeView::changeItemVisibility(bool show)
{
  QModelIndexList list = selectedIndexes();
  uint len = list.length();
  if (len == 0)
    return;

  assert(len >= 2);
  QModelIndex itemIndex = list.first();
  QModelIndex iconIndex = list.at(1);
  assert(itemIndex.row() == iconIndex.row());

  updateVisibilityIcon(itemIndex, iconIndex);

  if (show)
    showParentItems(m_pModel->itemFromIndex(itemIndex));
  changeChildrenVisibility(iconIndex, show);
}

void ModelTreeView::updateVisibilityIcon(const QModelIndex& itemIndex, const QModelIndex& iconIndex)
{
  QStandardItem* selectedItem = m_pModel->itemFromIndex(itemIndex);
  bool isVisible = false;
  QVariant data = selectedItem->data();
  if (data.isValid())
  {
    // get actual rengaapi::ModelObject visibility
    isVisible = isModelObjectVisible(data);
  }
  else
  {
    // get folder visibility
    if (selectedItem->rowCount() > 0)
    {
      // look for children items
      for (size_t i = 0; i < selectedItem->rowCount(); ++i)
      {
        QStandardItem* childItem = selectedItem->child(i);
        data = childItem->data();
        if (isModelObjectVisible(data))
        {
          isVisible = true;
          break;
        }
      }
    }
    else
    {
      // get level visibility
      QStandardItem* parent = selectedItem->parent();
      data = parent->data();
      isVisible = isModelObjectVisible(data);
    }
  }

  setIcon(iconIndex, isVisible);
}

void ModelTreeView::changeChildrenVisibility(const QModelIndex& iconIndex, bool visible)
{
  // change visibility of current item in renga if necessary
  const QModelIndex itemIndex = m_pModel->index(iconIndex.row(), 0, iconIndex.parent());
  QStandardItem* item = m_pModel->itemFromIndex(itemIndex);
  QVariant data = item->data();
  if (data.isValid())
  {
    setModelObjectVisibility(data, visible);
    // in some cases objects don't change visibility
    // for example, railing won't be shown if it's stairs hidden
    // so check actual object state
    visible = isModelObjectVisible(data);
  }

  if (item->rowCount() > 0)
  {
    // visit all children
    bool hasVisibleChild = false;
    for (size_t i = 0; i < item->rowCount(); ++i)
    {
      QModelIndex childItemIndex = item->child(i)->index();
      QModelIndex childIconIndex = m_pModel->index(childItemIndex.row(), 1, itemIndex);
      changeChildrenVisibility(childIconIndex, visible);
      hasVisibleChild |= m_pModel->itemFromIndex(childIconIndex)->data().toBool();
    }
    visible = hasVisibleChild;
  }

  // change visibility of current item in tree
  setIcon(iconIndex, visible);
}

void ModelTreeView::showParentItems(QStandardItem* child)
{
  QStandardItem* parent = child->parent();
  if (parent == nullptr)
    return;

  // make icon visible
  QModelIndex parentIndex = parent->index();
  QModelIndex parentIconIndex = m_pModel->index(parentIndex.row(), 1, parentIndex.parent());

  setIcon(parentIconIndex, true);

  // show object in renga
  QVariant parentData = parent->data();
  if (parentData.isValid() && !isModelObjectVisible(parentData))
    setModelObjectVisibility(parentData, true);

  showParentItems(parent);
}

bool ModelTreeView::isModelObjectVisible(const QVariant& data)
{
  assert(data.isValid());
  bool ok = false;
  uint id = data.toUInt(&ok);
  assert(ok);
  rengaapi::ObjectId objectId(id);
  return !rengaapi::ObjectVisibility::isVisibleIn3DView(objectId); // TODO: if (!visible) after bugfix
}

void ModelTreeView::setModelObjectVisibility(const QVariant& data, const bool visible)
{
  assert(data.isValid());
  bool ok = false;
  uint id = data.toUInt(&ok);
  assert(ok);
  rengaapi::ObjectIdCollection objectIdCollection;
  objectIdCollection.add(rengaapi::ObjectId(id));
  rengaapi::ObjectVisibility::setVisibleIn3DView(objectIdCollection, visible);
}

void ModelTreeView::setIcon(const QModelIndex iconIndex, bool visible)
{
  QStandardItem* iconItem = m_pModel->itemFromIndex(iconIndex);
  iconItem->setIcon(QIcon(visible ? ":/icons/Visible" : ":/icons/Hidden"));
  iconItem->setData(QVariant(visible));
}