#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include <string>
#include <boost/thread/shared_mutex.hpp>

#include <boost/shared_ptr.hpp>

namespace Platform {

class SharedData
{
protected:
	std::string m_data;

public:
	virtual ~SharedData() = default;

	virtual bool IsValid() const;
	virtual void Set(const std::string& data);
	virtual std::string Get() const;
};

class SharedDataSecured : public SharedData
{
	mutable boost::shared_mutex m_mtx;

	std::string (*m_key_function)();
	uint32_t m_hash;

	uint32_t GenerateHash() const;

	bool NonAtomicIsValid() const;

public:
	SharedDataSecured(const std::string& data, std::string(*key_function)());
	bool IsValid() const override;
	void Set(const std::string& data) override;
	std::string Get() const override;
};

typedef boost::shared_ptr<SharedData> SharedDataPtr;

} // namespace Platform

#endif
