#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include <string>

#include <boost/shared_ptr.hpp>

namespace Platform {

class SharedData
{
protected:
	std::string m_data;

public:
	virtual bool IsValid() const;
	virtual void Set(const std::string& data);
	virtual std::string Get() const;
};

class SharedDataSecured : public SharedData
{
	std::string (*m_key_function)();
	std::string m_hash;
	
	std::string GenerateHash() const;

public:
	SharedDataSecured(const std::string& data, std::string(*key_function)());
	bool IsValid() const override;
	void Set(const std::string& data) override;
	std::string Get() const override;
};

typedef boost::shared_ptr<SharedData> SharedDataPtr;

} // namespace Platform

#endif
