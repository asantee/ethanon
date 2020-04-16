#include "ETHCustomDataManager.h"
#include "../Util/ETHASUtil.h"
#include <iostream>

const std::string ETHCustomDataManager::DATA_NAME[ETHCustomData::CUSTOM_DATA_TYPE_COUNT] =
{
	(("")),
	(("float")),
	(("int")),
	(("uint")),
	(("string")),
	(("vector2")),
	(("vector3"))
};

const ETHCustomDataManager& ETHCustomDataManager::operator=(const ETHCustomDataManager& a)
{
	a.CopyMap(m_data);
	return a;
}

const std::string &ETHCustomDataManager::GetDataName(const unsigned int n)
{
	assert(n < ETHCustomData::CUSTOM_DATA_TYPE_COUNT);
	return DATA_NAME[n];
}

void ETHCustomDataManager::SetFloat(const std::string &name, const float &value)
{
	m_data[name] = ETHCustomDataPtr(new ETHFloatData(value));
}

void ETHCustomDataManager::SetInt(const std::string &name, const int &value)
{
	m_data[name] = ETHCustomDataPtr(new ETHIntData(value));
}

void ETHCustomDataManager::SetUInt(const std::string &name, const unsigned int &value)
{
	m_data[name] = ETHCustomDataPtr(new ETHUIntData(value));
}

void ETHCustomDataManager::SetString(const std::string &name, const std::string &sValue)
{
	m_data[name] = ETHCustomDataPtr(new ETHStringData(sValue));
}

void ETHCustomDataManager::SetVector2(const std::string &name, const Vector2 &v)
{
	m_data[name] = ETHCustomDataPtr(new ETHVector2Data(v));
}

void ETHCustomDataManager::SetVector3(const std::string &name, const Vector3 &v)
{
	m_data[name] = ETHCustomDataPtr(new ETHVector3Data(v));
}

bool ETHCustomDataManager::GetFloat(const std::string &name, float &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_FLOAT)
	{
		return false;
	}
	outValue = data->GetFloat();
	return true;
}

bool ETHCustomDataManager::GetInt(const std::string &name, int &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_INT)
	{
		return false;
	}
	outValue = data->GetInt();
	return true;
}

bool ETHCustomDataManager::GetUInt(const std::string &name, unsigned int &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_UINT)
	{
		return false;
	}
	outValue = data->GetUInt();
	return true;
}

bool ETHCustomDataManager::GetString(const std::string &name, std::string &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_STRING)
	{
		return false;
	}
	outValue = data->GetString();
	return true;
}

bool ETHCustomDataManager::GetVector2(const std::string &name, Vector2 &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR2)
	{
		return false;
	}
	outValue = data->GetVector2();
	return true;
}

bool ETHCustomDataManager::GetVector3(const std::string &name, Vector3 &outValue) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return false;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR3)
	{
		return false;
	}
	outValue = data->GetVector3();
	return true;
}

float ETHCustomDataManager::AddToFloat(const std::string &name, const float &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0.0f;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_FLOAT)
	{
		return 0.0f;
	}
	data->Set(value + data->GetFloat());
	return data->GetFloat();
}

int ETHCustomDataManager::AddToInt(const std::string &name, const int &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_INT)
	{
		return 0;
	}
	data->Set(value + data->GetInt());
	return data->GetInt();
}

unsigned int ETHCustomDataManager::AddToUInt(const std::string &name, const unsigned int &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_UINT)
	{
		return 0;
	}
	data->Set(value + data->GetUInt());
	return data->GetUInt();
}

Vector2 ETHCustomDataManager::AddToVector2(const std::string &name, const Vector2 &v)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return Vector2();
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR2)
	{
		return Vector2();
	}
	data->Set(v + data->GetVector2());
	return data->GetVector2();
}

Vector3 ETHCustomDataManager::AddToVector3(const std::string &name, const Vector3 &v)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return Vector3();
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR3)
	{
		return Vector3();
	}
	data->Set(v + data->GetVector3());
	return data->GetVector3();
}

float ETHCustomDataManager::MultiplyFloat(const std::string &name, const float &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0.0f;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_FLOAT)
	{
		return 0.0f;
	}
	data->Set(value * data->GetFloat());
	return data->GetFloat();
}

int ETHCustomDataManager::MultiplyInt(const std::string &name, const int &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_INT)
	{
		return 0;
	}
	data->Set(value * data->GetInt());
	return data->GetInt();
}

unsigned int ETHCustomDataManager::MultiplyUInt(const std::string &name, const unsigned int &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return 0;
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_UINT)
	{
		return 0;
	}
	data->Set(value * data->GetUInt());
	return data->GetUInt();
}

Vector2 ETHCustomDataManager::MultiplyVector2(const std::string &name, const float &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return Vector2();
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR2)
	{
		return Vector2();
	}
	data->Set(data->GetVector2() * value);
	return data->GetVector2();
}

Vector3 ETHCustomDataManager::MultiplyVector3(const std::string &name, const float &value)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return Vector3();
	}
	const ETHCustomDataPtr& data = iter->second;
	if (data->GetType() != ETHCustomData::DT_VECTOR3)
	{
		return Vector3();
	}
	data->Set(data->GetVector3() * value);
	return data->GetVector3();
}

ETHCustomData::DATA_TYPE ETHCustomDataManager::Check(const std::string &name) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter != m_data.end())
	{
		return iter->second->GetType();
	}
	else
	{
		return ETHCustomData::DT_NODATA;
	}
}

std::string ETHCustomDataManager::GetDebugStringData() const
{
	if (!m_data.empty())
	{
		std::stringstream ss;
		for (tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.begin(); iter != m_data.end(); ++iter)
		{
			ss << DATA_NAME[iter->second->GetType()] << (" ") << iter->first
				<< (" = ") << iter->second->GetValueAsString() << std::endl;
		}
		return ss.str();
	}
	else
	{
		return ("");
	}
}

bool ETHCustomDataManager::HasData() const
{
	return (m_data.size()>0);
}

bool ETHCustomDataManager::EraseData(const std::string &name)
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::iterator iter = m_data.find(name);
	if (iter != m_data.end())
	{
		m_data.erase(iter);
	}
	else
	{
		return false;
	}
	return true;
}

unsigned int ETHCustomDataManager::GetNumVariables() const
{
	return static_cast<unsigned int>(m_data.size());
}

void ETHCustomDataManager::Clear()
{
	m_data.clear();
}

void ETHCustomDataManager::MoveData(ETHCustomDataManager &dataOut) const
{
	
	for (tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.begin();
		 iter != m_data.end(); ++iter)
	{
		dataOut.AddData(iter->first, iter->second);
	}
}

void ETHCustomDataManager::InsertData(const ETHCustomDataManager &dataIn)
{
	for (tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = dataIn.m_data.begin();
		 iter != dataIn.m_data.end(); ++iter)
	{
		AddData(iter->first, iter->second);
	}
}

void ETHCustomDataManager::CopyMap(tsl::hopscotch_map<std::string, ETHCustomDataPtr> &inMap) const
{
	inMap.clear();
	for (tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.begin(); iter != m_data.end(); ++iter)
	{
		const ETHCustomDataPtr& data = iter->second;
		const std::string& name = iter->first;
		switch (data->GetType())
		{
		case ETHCustomData::DT_FLOAT:
			inMap[name] = ETHCustomDataPtr(new ETHFloatData(data->GetFloat()));
			break;
		case ETHCustomData::DT_INT:
			inMap[name] = ETHCustomDataPtr(new ETHIntData(data->GetInt()));
			break;
		case ETHCustomData::DT_UINT:
			inMap[name] = ETHCustomDataPtr(new ETHUIntData(data->GetUInt()));
			break;
		case ETHCustomData::DT_STRING:
			inMap[name] = ETHCustomDataPtr(new ETHStringData(data->GetString()));
			break;
		case ETHCustomData::DT_VECTOR2:
			inMap[name] = ETHCustomDataPtr(new ETHVector2Data(data->GetVector2()));
			break;
		case ETHCustomData::DT_VECTOR3:
			inMap[name] = ETHCustomDataPtr(new ETHVector3Data(data->GetVector3()));
			break;
		default:
			break;
		};
	}
}

void ETHCustomDataManager::AddData(const std::string &name, const ETHCustomDataConstPtr &dataIn)
{
	switch (dataIn->GetType())
	{
		case ETHCustomData::DT_FLOAT:
			m_data[name] = ETHCustomDataPtr(new ETHFloatData(dataIn->GetFloat()));
			break;
		case ETHCustomData::DT_INT:
			m_data[name] = ETHCustomDataPtr(new ETHIntData(dataIn->GetInt()));
			break;
		case ETHCustomData::DT_UINT:
			m_data[name] = ETHCustomDataPtr(new ETHUIntData(dataIn->GetUInt()));
			break;
		case ETHCustomData::DT_STRING:
			m_data[name] = ETHCustomDataPtr(new ETHStringData(dataIn->GetString()));
			break;
		case ETHCustomData::DT_VECTOR2:
			m_data[name] = ETHCustomDataPtr(new ETHVector2Data(dataIn->GetVector2()));
			break;
		case ETHCustomData::DT_VECTOR3:
			m_data[name] = ETHCustomDataPtr(new ETHVector3Data(dataIn->GetVector3()));
			break;
		default:
			break;
	};
}

std::string ETHCustomDataManager::GetValueAsString(const std::string &name) const
{
	tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.find(name);
	if (iter == m_data.end())
	{
		return ("");
	}
	else
	{
		return iter->second->GetValueAsString();
	}
}

bool ETHCustomDataManager::ReadDataFromXMLFile(TiXmlElement *pRoot)
{
	TiXmlNode *pNode = pRoot->FirstChild(("CustomData"));
	if (pNode)
	{
		TiXmlElement *pEntities = pNode->ToElement();
		if (pEntities)
		{
			pNode = pEntities->FirstChild(("Variable"));
			if (pNode)
			{
				TiXmlElement *pVarIter = pNode->ToElement();
				if (pVarIter)
				{
					do
					{
						std::string type, name, value;
						TiXmlElement *pElement;

						// read the variable type (as a string)
						pNode = pVarIter->FirstChild(("Type"));
						if (pNode)
						{
							pElement = pNode->ToElement();
							if (pElement)
							{
								type = pElement->GetText();
							}
						}

						// read the variable name
						pNode = pVarIter->FirstChild(("Name"));
						if (pNode)
						{
							pElement = pNode->ToElement();
							if (pElement)
							{
								name = pElement->GetText();
							}
						}

						// read the variable value
						if (type != ("") && name != (""))
						{
							pNode = pVarIter->FirstChild(("Value"));
							if (pNode)
							{
								pElement = pNode->ToElement();
								if (pElement)
								{
									if (type == DATA_NAME[ETHCustomData::DT_VECTOR2])
									{
										Vector2 value(0,0);
										pElement->QueryFloatAttribute(("x"), &value.x);
										pElement->QueryFloatAttribute(("y"), &value.y);
										SetVector2(name, value);
									}
									else if (type == DATA_NAME[ETHCustomData::DT_VECTOR3])
									{
										Vector3 value(0,0,0);
										pElement->QueryFloatAttribute(("x"), &value.x);
										pElement->QueryFloatAttribute(("y"), &value.y);
										pElement->QueryFloatAttribute(("z"), &value.z);
										SetVector3(name, value);
									}
									else
									{
										value = pElement->GetText();

										// passing the variable to the map
										if (type == DATA_NAME[ETHCustomData::DT_FLOAT])
										{
											SetFloat(name, ETHGlobal::ParseFloat(value.c_str()));
										} else
										if (type == DATA_NAME[ETHCustomData::DT_INT])
										{
											SetInt(name, ETHGlobal::ParseInt(value.c_str()));
										} else
										if (type == DATA_NAME[ETHCustomData::DT_UINT])
										{
											SetUInt(name, ETHGlobal::ParseUInt(value.c_str()));
										} else
										if (type == DATA_NAME[ETHCustomData::DT_STRING])
										{
											SetString(name, value);
										}
									}
								}
							}
						}

						pVarIter = pVarIter->NextSiblingElement();
					} while (pVarIter);
				}
			}
		}
	}
	return true;
}

bool ETHCustomDataManager::WriteDataToFile(TiXmlElement *pHeadRoot) const
{
	TiXmlElement *pCustomData = new TiXmlElement(("CustomData"));
	pHeadRoot->LinkEndChild(pCustomData);
	for (tsl::hopscotch_map<std::string, ETHCustomDataPtr>::const_iterator iter = m_data.begin();
		iter != m_data.end(); ++iter)
	{
		TiXmlElement *pVariableRoot = new TiXmlElement(("Variable"));
		pCustomData->LinkEndChild(pVariableRoot); 

		const ETHCustomDataPtr& data = iter->second;

		TiXmlElement *pElement;
		pElement = new TiXmlElement(("Type"));
		pElement->LinkEndChild(new TiXmlText(DATA_NAME[data->GetType()] ));
		pVariableRoot->LinkEndChild(pElement);

		pElement = new TiXmlElement(("Name"));
		pElement->LinkEndChild(new TiXmlText(iter->first));
		pVariableRoot->LinkEndChild(pElement);

		pElement = new TiXmlElement(("Value"));

		std::stringstream ss;
		switch (data->GetType())
		{
		case ETHCustomData::DT_FLOAT:
			ss << data->GetFloat();
			break;
		case ETHCustomData::DT_INT:
			ss << data->GetInt();
			break;
		case ETHCustomData::DT_UINT:
			ss << data->GetUInt();
			break;
		case ETHCustomData::DT_STRING:
			ss << data->GetString();
			break;
		default:
			break;
		};

		switch (data->GetType())
		{
		case ETHCustomData::DT_FLOAT:
		case ETHCustomData::DT_INT:
		case ETHCustomData::DT_UINT:
		case ETHCustomData::DT_STRING:
			pElement->LinkEndChild(new TiXmlText(ss.str()));
			break;
		case ETHCustomData::DT_VECTOR2:
			pElement->SetDoubleAttribute(("x"), data->GetVector2().x);
			pElement->SetDoubleAttribute(("y"), data->GetVector2().y);
			break;
		case ETHCustomData::DT_VECTOR3:
			pElement->SetDoubleAttribute(("x"), data->GetVector3().x);
			pElement->SetDoubleAttribute(("y"), data->GetVector3().y);
			pElement->SetDoubleAttribute(("z"), data->GetVector3().z);
			break;
		default:
			break;
		};
		pVariableRoot->LinkEndChild(pElement);
	}

	return true;
}
