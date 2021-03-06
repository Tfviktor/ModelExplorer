//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#pragma once
#include "PropertyManagers.h"
#include "IPropertyViewBuilder.h"
#include "IPropertyViewSourceObject.h"

#include <qttreepropertybrowser.h>


class PropertyView : public QtTreePropertyBrowser
{
  Q_OBJECT
    
public:
  enum Mode
  {
    CategoryMode = 0,
    ListMode = 1,
  };

  PropertyView(QWidget* pParent, Renga::IApplicationPtr pApplication);

  void showProperties(IPropertyViewSourceObject* pSourceObject);

  void changeMode(PropertyView::Mode newMode);

private slots:
  void userDoubleAttributeChanged(QtProperty* property, const QString& newValue);
  void userStringAttributeChanged(QtProperty* property, const QString& newValue);

  void parameterBoolChanged(QtProperty* pProperty, bool val);
  void parameterIntChanged(QtProperty* pProperty, int val);
  void parameterDoubleChanged(QtProperty* pProperty, const QString& newValue);
  void parameterStringChanged(QtProperty* pProperty, const QString& newValue);

private:
  void updateParameters();

  void initPropertyManagers();
  void clearPropertyManagers();

  bool createProperties(
    IPropertyViewBuilder* pObjectPropertyViewBuilder,
    PropertyList& integratedParameters,
    PropertyList& parameters,
    PropertyList& quantities,
    PropertyList& properties);

  void buildPropertyViewAsList(PropertyList& integratedParameters, PropertyList& parameters, PropertyList& quantities, PropertyList& properties);
  void buildPropertyViewByCategory(const PropertyList& integratedParameters, PropertyList& parameters, const PropertyList& quantities, const PropertyList& properties);
  void buildPropertyViewSingleCategory(const QString& categoryName, const PropertyList& categoryProperties);

  void buildPropertyView(IPropertyViewSourceObject* pSourceObject);

  Renga::IPropertyPtr getProperty(QtProperty* property);
  Renga::IOperationPtr createOperation();

  void resetUserAttribute(QtProperty* property);
  void changeUserAttribute(QtProperty* property, const double value);
  void changeUserAttribute(QtProperty* property, const std::wstring& value);

private:
  Renga::IApplicationPtr m_pApplication;
  QtGroupPropertyManager* m_pGroupManager;
  PropertyManagers m_propertyManagers;
  Mode m_propertyViewMode;

  std::unique_ptr<IPropertyViewSourceObject> m_pSourceObject;
};
