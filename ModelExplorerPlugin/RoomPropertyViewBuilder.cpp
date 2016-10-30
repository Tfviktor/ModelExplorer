//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "RoomPropertyViewBuilder.h"

#include <RengaAPI/Room.h>

RoomPropertyViewBuilder::RoomPropertyViewBuilder(const PropertyManagers* pPropertyManagers, const QTranslator* pTranslator) 
  : ObjectPropertyViewBuilder(pPropertyManagers, pTranslator)
{}

PropertyMap RoomPropertyViewBuilder::createParametersProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::Room* pRoom = dynamic_cast<rengaapi::Room*>(pObject);
	PropertyMap result;

	// name, roomNumber, offset
	QtProperty* name = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("room", "name"));
	QtProperty* roomNumber = m_pPropertyManagers->m_pStringManager->addProperty(m_pTranslator->translate("room", "roomNumber"));
	QtProperty* offset = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("room", "offset"));

	m_pPropertyManagers->m_pStringManager->setValue(name, rengaStringToQString(pRoom->roomName()));
	m_pPropertyManagers->m_pStringManager->setValue(roomNumber, rengaStringToQString(pRoom->roomNumber()));
  m_pPropertyManagers->m_pDoubleManager->setValue(offset, pRoom->offset().inMillimeters());

	result.insert(std::make_pair(name->propertyName(), name));
	result.insert(std::make_pair(roomNumber->propertyName(), roomNumber));
	result.insert(std::make_pair(offset->propertyName(), offset));

  return result;
}


PropertyMap RoomPropertyViewBuilder::createQuantitiesProperties(rengaapi::ModelObject* pObject)
{
	rengaapi::Room* pRoom = dynamic_cast<rengaapi::Room*>(pObject);
	rengaapi::RoomQuantities roomQuantities = pRoom->quantities();

  PropertyMap result;

	// nominalHeight, grossVolume, grossFloorArea, grossPerimeter
  QtProperty* nominalHeight = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("room", "nominalHeight"));
	QtProperty* grossVolume = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("room", "grossVolume"));
	QtProperty* grossFloorArea = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("room", "grossFloorArea"));
	QtProperty* grossPerimeter = m_pPropertyManagers->m_pDoubleManager->addProperty(m_pTranslator->translate("room", "grossPerimeter"));

  setLengthMeasureOptional(result, roomQuantities.nominalHeight(), nominalHeight);
	setVolumeMeasureOptional(result, roomQuantities.grossVolume(), grossVolume);
	setAreaMeasureOptional(result, roomQuantities.grossFloorArea(), grossFloorArea);
	setLengthMeasureOptional(result, roomQuantities.grossPerimeter(), grossPerimeter);

  return result;
}