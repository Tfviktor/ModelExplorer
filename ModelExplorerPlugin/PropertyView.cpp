//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "PropertyView.h"
#include "GuidUtils.h"

#include <qteditorfactory.h>
#include "PropertyViewModelObjectSource.h"

#include <Windows.h>

PropertyView::PropertyView(QWidget* pParent, Renga::IApplicationPtr pApplication) :
  QtTreePropertyBrowser(pParent),
  m_pApplication(pApplication),
  m_propertyViewMode(CategoryMode)
{
  m_pGroupManager = new QtGroupPropertyManager(this);

  initPropertyManagers();
}

void PropertyView::showProperties(IPropertyViewSourceObject* pSourceObject)
{
  m_pSourceObject.reset(pSourceObject);

  buildPropertyView(pSourceObject);
}

void PropertyView::changeMode(PropertyView::Mode newMode)
{
  m_propertyViewMode = newMode;

  buildPropertyView(m_pSourceObject.get());
}

void PropertyView::initPropertyManagers()
{
  m_propertyManagers.init(this);

   // handle user attributes changes

  {
    auto& mngr = m_propertyManagers.m_parameters;
    QObject::connect(mngr.m_pBoolManager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(parameterBoolChanged(QtProperty*, bool)));
    QObject::connect(mngr.m_pIntManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(parameterIntChanged(QtProperty*, int)));
    QObject::connect(mngr.m_pDoubleManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(parameterDoubleChanged(QtProperty*, const QString&)));
    QObject::connect(mngr.m_pStringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(parameterStringChanged(QtProperty*, const QString&)));
  }

   QObject::connect(
     m_propertyManagers.m_properties.m_pDoubleManager, SIGNAL(valueChanged(QtProperty*, const QString&)),
     this, SLOT(userDoubleAttributeChanged(QtProperty*, const QString&)));

   QObject::connect(
     m_propertyManagers.m_properties.m_pStringManager, SIGNAL(valueChanged(QtProperty*, const QString&)),
     this, SLOT(userStringAttributeChanged(QtProperty*, const QString&)));
}

void PropertyView::clearPropertyManagers()
{
  m_propertyManagers.clear();
  m_pGroupManager->clear();
}

void PropertyView::buildPropertyViewAsList(PropertyList& parameters, PropertyList& parametersEx, PropertyList& calculated, PropertyList& userDefinedProperties)
{
  // show in list mode
  PropertyList allProperties;
  allProperties.splice(allProperties.end(), parameters);
  allProperties.splice(allProperties.end(), parametersEx);
  allProperties.splice(allProperties.end(), calculated);
  allProperties.splice(allProperties.end(), userDefinedProperties);

  allProperties.sort([](QtProperty* left, QtProperty* right) -> bool
                    {
                    return left->propertyName() < right->propertyName();
                    });

  for (auto it = allProperties.begin(); it != allProperties.end(); ++it)
    addProperty(*it);
}

void PropertyView::buildPropertyViewSingleCategory(const QString& categoryName, const PropertyList& categoryProperties)
{
  if(categoryProperties.empty())
    return;

  QtProperty* pCategoryProperty = m_pGroupManager->addProperty(categoryName);
  pCategoryProperty->setBold(true);
  
  PropertyList singleCategoryProperties(categoryProperties);
  singleCategoryProperties.sort([](QtProperty* left, QtProperty* right) -> bool
                               {
                                 return left->propertyName() < right->propertyName();
                               });

  for (auto it = singleCategoryProperties.begin(); it != singleCategoryProperties.end(); ++it)
    pCategoryProperty->addSubProperty(*it);

  addProperty(pCategoryProperty);
}

void PropertyView::buildPropertyView(IPropertyViewSourceObject* pSourceObject)
{
  clearPropertyManagers();

  if (pSourceObject == nullptr)
    return;

  auto propertyViewBuilder = pSourceObject->createPropertyViewBuilder(&m_propertyManagers);

  PropertyList integratedParameters;

  PropertyList parameters;
  PropertyList quantities;
  PropertyList properties;

  if (!createProperties(propertyViewBuilder.get(), integratedParameters, parameters, quantities, properties))
    return;

  if (m_propertyViewMode == Mode::ListMode)
    buildPropertyViewAsList(integratedParameters, parameters, quantities, properties);
  else
    buildPropertyViewByCategory(integratedParameters, parameters, quantities, properties);
}

void PropertyView::buildPropertyViewByCategory(const PropertyList& integratedParameters, PropertyList& parameters, const PropertyList& quantities, const PropertyList& properties)
{
  buildPropertyViewSingleCategory(QApplication::translate("me_propertyView", "Object parameters"), integratedParameters);
  buildPropertyViewSingleCategory(QApplication::translate("me_propertyView", "Object parameters ex"), parameters);
  buildPropertyViewSingleCategory(QApplication::translate("me_propertyView", "Calculated characteristics"), quantities);
  buildPropertyViewSingleCategory(QApplication::translate("me_propertyView", "Object properties"), properties);
}

bool PropertyView::createProperties(
  IPropertyViewBuilder* pObjectPropertyViewBuilder,
  PropertyList& integratedParameters,
  PropertyList& parameters,
  PropertyList& quantities,
  PropertyList& properties)
{
  if (pObjectPropertyViewBuilder == nullptr)
    return false;

  integratedParameters.clear();

  parameters.clear();
  quantities.clear();

  pObjectPropertyViewBuilder->createIntegratedParameters(integratedParameters);

  pObjectPropertyViewBuilder->createParameters(parameters);
  pObjectPropertyViewBuilder->createQuantities(quantities);

  properties = pObjectPropertyViewBuilder->createProperties();

  return true;
}

void PropertyView::updateParameters() {
  buildPropertyView(m_pSourceObject.get());
}

void PropertyView::parameterBoolChanged(QtProperty* pProperty, bool val)
{
  const auto parameterId = GuidFromString(pProperty->data().toStdString());

  auto pParameter = m_pSourceObject->getParameter(parameterId);
  if (!pParameter)
    return;

  if (pParameter->GetValueType() != Renga::ParameterValueType::ParameterValueType_Bool)
    return;

  if (auto pOperation = createOperation())
  {
    pOperation->Start();
    pParameter->SetBoolValue(val);
    pOperation->Apply();

    updateParameters();
  }
}

void PropertyView::parameterIntChanged(QtProperty* pProperty, int val)
{
  const auto parameterId = GuidFromString(pProperty->data().toStdString());

  auto pParameter = m_pSourceObject->getParameter(parameterId);
  if (!pParameter)
    return;

  if (pParameter->GetValueType() != Renga::ParameterValueType::ParameterValueType_Int)
    return;

  if (auto pOperation = createOperation())
  {
    pOperation->Start();
    pParameter->SetIntValue(val);
    pOperation->Apply();

    updateParameters();
  }
}

void PropertyView::parameterDoubleChanged(QtProperty* pProperty, const QString& newValue)
{
  if (!newValue.isEmpty())
  {
    bool ok = false;
    double newDoubleValue = QLocale::system().toDouble(newValue, &ok);
    if (ok)
    {
      const auto parameterId = GuidFromString(pProperty->data().toStdString());

      auto pParameter = m_pSourceObject->getParameter(parameterId);
      if (!pParameter)
        return;

      if (pParameter->GetValueType() != Renga::ParameterValueType::ParameterValueType_Double)
        return;

      if (auto pOperation = createOperation())
      {
        pOperation->Start();
        pParameter->SetDoubleValue(newDoubleValue);
        pOperation->Apply();

        updateParameters();
      }
    }
  }
}

void PropertyView::parameterStringChanged(QtProperty* pProperty, const QString& newValue)
{
  const auto parameterId = GuidFromString(pProperty->data().toStdString());

  auto pParameter = m_pSourceObject->getParameter(parameterId);
  if (!pParameter)
    return;

  if (pParameter->GetValueType() != Renga::ParameterValueType::ParameterValueType_String)
    return;

  if (auto pOperation = createOperation())
  {
    pOperation->Start();
    pParameter->SetStringValue(newValue.toStdWString().c_str());
    pOperation->Apply();

    updateParameters();
  }
}

void PropertyView::userDoubleAttributeChanged(QtProperty* userAttributeProperty, const QString& newValue)
{
  if (newValue.isEmpty())
    resetUserAttribute(userAttributeProperty); // will reset the attribute value
  else
  {
    bool ok = false;
    double newDoubleValue = QLocale::system().toDouble(newValue, &ok);
    if (ok)
      changeUserAttribute(userAttributeProperty, newDoubleValue);
  }
}

void PropertyView::userStringAttributeChanged(QtProperty* userAttributeProperty, const QString& newValue)
{
  if (newValue.isEmpty())
    resetUserAttribute(userAttributeProperty); // will reset the attribute value
  else
    changeUserAttribute(userAttributeProperty, newValue.toStdWString());
}

Renga::IPropertyPtr PropertyView::getProperty(QtProperty* userAttributeProperty)
{
  if (m_pSourceObject == nullptr)
    return nullptr;

  const auto propertyId = GuidFromString(userAttributeProperty->data().toStdString());
     
  return m_pSourceObject->getUserDefinedProperty(propertyId);
}

Renga::IOperationPtr PropertyView::createOperation()
{
  auto pProject = m_pApplication->GetProject();
  auto pModel = pProject->GetModel();
  return pModel->CreateOperation();
}

void PropertyView::resetUserAttribute(QtProperty* userAttributeProperty)
{
  auto pProperty = getProperty(userAttributeProperty);
  if (!pProperty)
    return;

  auto pOperation = createOperation();
  pOperation->Start();

  pProperty->ResetValue();

  pOperation->Apply();
}

void PropertyView::changeUserAttribute(QtProperty* userAttributeProperty, const double value)
{
  auto pProperty = getProperty(userAttributeProperty);
  if (!pProperty)
    return;

  if (pProperty->GetType() != Renga::PropertyType::PropertyType_Double)
    return;

  auto pOperation = createOperation();

  pOperation->Start();
  pProperty->SetDoubleValue(value);
  pOperation->Apply();
}

void PropertyView::changeUserAttribute(QtProperty* userAttributeProperty, const std::wstring& value)
{
  auto pProperty = getProperty(userAttributeProperty);
  if (!pProperty)
    return;

  if (pProperty->GetType() != Renga::PropertyType::PropertyType_String)
    return;

  auto pOperation = createOperation();

  pOperation->Start();
  pProperty->SetStringValue(value.c_str());
  pOperation->Apply();
}
