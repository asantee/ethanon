#include "ETHPhysicsSimulator.h"
#include "ETHRayCastCallback.h"

#include <boost/shared_array.hpp>

const b2Vec2 ETHPhysicsSimulator::DEFAULT_GRAVITY(0, 10);
const float ETHPhysicsSimulator::DEFAULT_SCALE(50.0f);
int32 ETHPhysicsSimulator::m_velocityIterations(5);
int32 ETHPhysicsSimulator::m_positionIterations(2);

std::vector<b2Shape*> ETHPhysicsSimulator::GetBoxShape(const ETHCollisionBox& box, const float angle)
{
	b2PolygonShape* boxShape = new b2PolygonShape;
	const b2Vec2 size = ScaleToBox2D(Vector2(box.size.x, box.size.y) * 0.5f);
	boxShape->SetAsBox(size.x, size.y, ScaleToBox2D(ETHGlobal::ToVector2(box.pos)), angle);
	std::vector<b2Shape*> r; if (boxShape) r.push_back(boxShape);
	return r;
}

std::vector<b2Shape*> ETHPhysicsSimulator::GetPolygonShape(const ETHCollisionBox& box, const ETHPolygonPtr& polygon)
{
	const std::size_t numVertices = polygon->GetNumVertices();
	const std::vector<Vector2>& polygonVertices = polygon->GetVertices();
	boost::shared_array<b2Vec2> vertices = boost::shared_array<b2Vec2>(new b2Vec2 [numVertices]);
	for (std::size_t t = 0; t < numVertices; t++)
	{
		vertices[t] = ScaleToBox2D(((Vector2(box.size.x, box.size.y) * 0.5f) * polygonVertices[t]) + Vector2(box.pos.x, box.pos.y));
	}

	b2PolygonShape* polygonShape = new b2PolygonShape;
	polygonShape->Set(vertices.get(), static_cast<unsigned int>(numVertices));
	std::vector<b2Shape*> r; if (polygonShape) r.push_back(polygonShape);
	return r;
}

std::vector<b2Shape*> ETHPhysicsSimulator::GetCircleShape(const ETHCollisionBox& box)
{
	b2CircleShape* circle = new b2CircleShape;
	circle->m_radius = ScaleToBox2D((box.size.x + box.size.y) / 4.0f);
	circle->m_p = ScaleToBox2D(ETHGlobal::ToVector2(box.pos));
	std::vector<b2Shape*> r; if (circle) r.push_back(circle);
	return r;
}

b2Body* ETHPhysicsSimulator::CreateBody(ETHEntity *entity, const boost::shared_ptr<b2World>& world)
{
	b2BodyDef bodyDef; 
	bodyDef.type = entity->IsStatic() ? b2_staticBody : b2_dynamicBody;
	bodyDef.userData = entity; entity->AddRef();
	bodyDef.fixedRotation = entity->IsFixedRotation();

	ETHCollisionBox box = entity->GetCollisionBox();
	box.Scale(entity->GetScale());
	const b2Vec2 pos = ScaleToBox2D(entity->GetPositionXY());
	bodyDef.position.Set(pos.x, pos.y);
	bodyDef.angle =-(Util::DegreeToRadian(entity->GetAngle()));
	bodyDef.bullet = entity->IsBullet();
	bodyDef.gravityScale = entity->GetProperties()->gravityScale;

	b2Body* body = world->CreateBody(&bodyDef);

	std::vector<b2Shape*> shapes;
	switch (entity->GetShape())
	{
	case ETHEntityProperties::BS_CIRCLE:
		shapes = GetCircleShape(box);
		break;
	case ETHEntityProperties::BS_POLYGON:
		shapes = GetPolygonShape(box, entity->GetPolygon());
		break;
	case ETHEntityProperties::BS_COMPOUND:
		shapes = entity->GetCompoundShape()->GetShapes(ETHGlobal::ToVector2(box.size));
		break;
	case ETHEntityProperties::BS_BOX:
	default:
		shapes = GetBoxShape(box);
		break;
	}

	if (!shapes.empty())
	{
		for (std::size_t t = 0; t < shapes.size(); t++)
		{
			const unsigned int idx = static_cast<unsigned int>(t);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = shapes[t];
			fixtureDef.density = entity->IsStatic() ? 0.0f : entity->GetDensity(idx);
			fixtureDef.friction = entity->GetFriction(idx);
			fixtureDef.isSensor = entity->IsSensor();
			fixtureDef.restitution = entity->GetRestitution(idx);
			body->CreateFixture(&fixtureDef);
			delete shapes[t];
		}
		shapes.clear();
		return body;
	}
	else
	{
		return 0;
	}
}

ETHPhysicsSimulator::ETHPhysicsSimulator(const float currentFpsRate) :
	m_timeStepScale(1.0f),
	m_fixedTimeStep(false),
	m_fixedTimeStepValue(1.0f / 60.0f),
	m_dynamicTimeStep(1.0f / currentFpsRate),
	m_timeStepUpdateTime(0.0f)
{
	const bool doSleep = true; // just making it more readable
	m_world = boost::shared_ptr<b2World>(new b2World(DEFAULT_GRAVITY, doSleep));
	m_world->SetContactListener(&m_contactListener);
	m_world->SetDestructionListener(&m_destructionListener);
}

ETHPhysicsSimulator::~ETHPhysicsSimulator()
{
	m_world->SetContactListener(NULL);
	m_world->SetDestructionListener(NULL);
	m_world.reset();
}

ETHPhysicsEntityControllerPtr ETHPhysicsSimulator::CreatePhysicsController(ETHEntity *entity, asIScriptModule* module, asIScriptContext* context)
{
	return CreatePhysicsController(entity, m_world, module, context);
}

ETHPhysicsEntityControllerPtr ETHPhysicsSimulator::CreatePhysicsController(
	ETHEntity *entity, const boost::shared_ptr<b2World>& world, asIScriptModule* module, asIScriptContext* context)
{
	b2Body *body = CreateBody(entity, world);
	return ETHPhysicsEntityControllerPtr(new ETHPhysicsEntityController(entity->GetController(), body, world, module, context));
}

void ETHPhysicsSimulator::Update(const float lastFrameElapsedTime)
{
	m_dynamicTimeStep = (static_cast<float32>(lastFrameElapsedTime) / 1000.0f);
	const float step = (!m_fixedTimeStep) ? m_dynamicTimeStep : m_fixedTimeStepValue;
	m_world->Step(step * m_timeStepScale, m_velocityIterations, m_positionIterations);

	m_contactListener.RunAndClearStackedEndContactCallbacks();
}

float ETHPhysicsSimulator::GetCurrentDynamicTimeStepMS() const
{
	return (m_fixedTimeStep) ? m_fixedTimeStepValue * 1000.0f : m_dynamicTimeStep * 1000.0f;
}

b2Vec2 ETHPhysicsSimulator::ScaleToBox2D(const Vector2& v)
{
	return b2Vec2(v.x / DEFAULT_SCALE, v.y / DEFAULT_SCALE);
}

Vector2 ETHPhysicsSimulator::ScaleFromBox2D(const b2Vec2& v)
{
	return Vector2(v.x * DEFAULT_SCALE, v.y * DEFAULT_SCALE);
}

float32 ETHPhysicsSimulator::ScaleToBox2D(const float& v)
{
	return v / DEFAULT_SCALE;
}

float ETHPhysicsSimulator::ScaleFromBox2D(const float32& v)
{
	return v * DEFAULT_SCALE;
}

void ETHPhysicsSimulator::SetNumIterations(const int velocityIterations, const int positionIterations)
{
	m_velocityIterations = velocityIterations;
	m_positionIterations = positionIterations;
}

void ETHPhysicsSimulator::GetNumIterations(int& velocityIterations, int& positionIterations)
{
	velocityIterations = m_velocityIterations;
	positionIterations = m_positionIterations;
}

void ETHPhysicsSimulator::SetGravity(const Vector2& gravity, const boost::shared_ptr<b2World>& world)
{
	world->SetGravity(b2Vec2(gravity.x, gravity.y));
}

Vector2 ETHPhysicsSimulator::GetGravity(const boost::shared_ptr<b2World>& world)
{
	const b2Vec2 gravity = world->GetGravity();
	return Vector2(gravity.x, gravity.y);
}

void ETHPhysicsSimulator::SetGravity(const Vector2& gravity)
{
	SetGravity(gravity, m_world);
}

Vector2 ETHPhysicsSimulator::GetGravity() const
{
	return GetGravity(m_world);
}

void ETHPhysicsSimulator::SetTimeStepScale(const float scale)
{
	m_timeStepScale = scale;
}

float ETHPhysicsSimulator::GetTimeStepScale() const
{
	return m_timeStepScale;
}

ETHEntity* ETHPhysicsSimulator::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal)
{
	if (a == b)
		return 0;
	ETHEntityDefaultChooser chooser;
	ETHRayCastCallback rayCastCallback(a, b, &chooser);
	const b2Vec2& p1 = rayCastCallback.GetScaledA();
	const b2Vec2& p2 = rayCastCallback.GetScaledB();

	// verify it again since values have been scaled and this
	// is an assert test in the box2D code
	if (b2Vec2(p2 - p1).LengthSquared() <= 0.0f)
		return 0;

	m_world->RayCast(&rayCastCallback, p1, p2);
	return rayCastCallback.GetClosestContact(point, normal);
}

ETHEntity* ETHPhysicsSimulator::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal, const str_type::string& semicolonSeparatedIgnoreList)
{
	if (a == b)
		return 0;
	ETHEntityNameArrayChooser chooser(semicolonSeparatedIgnoreList, true);
	ETHRayCastCallback rayCastCallback(a, b, &chooser);
	
	const b2Vec2& p1 = rayCastCallback.GetScaledA();
	const b2Vec2& p2 = rayCastCallback.GetScaledB();
	
	// verify it again since values have been scaled and this
	// is an assert test in the box2D code
	if (b2Vec2(p2 - p1).LengthSquared() <= 0.0f)
		return 0;
	
	m_world->RayCast(&rayCastCallback, p1, p2);
	return rayCastCallback.GetClosestContact(point, normal);
}

bool ETHPhysicsSimulator::GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities)
{
	if (a == b)
		return false;
	ETHEntityDefaultChooser chooser;
	ETHRayCastCallback rayCastCallback(a, b, &chooser);
	
	const b2Vec2& p1 = rayCastCallback.GetScaledA();
	const b2Vec2& p2 = rayCastCallback.GetScaledB();
	
	// verify it again since values have been scaled and this
	// is an assert test in the box2D code
	if (b2Vec2(p2 - p1).LengthSquared() <= 0.0f)
		return false;
	
	m_world->RayCast(&rayCastCallback, p1, p2);
	return rayCastCallback.GetContactEntities(entities);
}

bool ETHPhysicsSimulator::IsFixedTimeStep() const
{
	return m_fixedTimeStep;
}

float ETHPhysicsSimulator::GetFixedTimeStepValue() const
{
	return m_fixedTimeStepValue;
}

void ETHPhysicsSimulator::SetFixedTimeStep(const bool enable)
{
	m_fixedTimeStep = enable;
}

void ETHPhysicsSimulator::SetFixedTimeStepValue(const float value)
{
	m_fixedTimeStepValue = value;
}

ETHEntityProperties::BODY_SHAPE ETHPhysicsSimulator::StringToShape(const gs2d::str_type::string& str)
{
	if (str == GS_L("box"))
		return ETHEntityProperties::BS_BOX;
	else if (str == GS_L("circle"))
		return ETHEntityProperties::BS_CIRCLE;
	else if (str == GS_L("polygon"))
		return ETHEntityProperties::BS_POLYGON;
	else if (str == GS_L("compound"))
		return ETHEntityProperties::BS_COMPOUND;
	else
		return ETHEntityProperties::BS_NONE;
}

b2Joint* ETHPhysicsSimulator::CreateJoint(b2JointDef& jointDef)
{
	return m_world->CreateJoint(&jointDef);
}

void ETHPhysicsSimulator::ResolveJoints(ETHEntityArray& entities)
{
	const unsigned int numEntities = entities.size();
	for (unsigned int t = 0; t < numEntities; t++)
	{
		ETHPhysicsEntityControllerPtr controller = boost::dynamic_pointer_cast<ETHPhysicsEntityController>(entities[t]->GetController());
		if (controller)
		{
			controller->ResolveJoints(entities, *entities[t]->GetProperties(), *this);
		}
	}
}

void ETHPhysicsSimulator::DisableNextContact()
{
	m_contactListener.DisableNextContact();
}

bool ETHPhysicsSimulator::IsRunningPreSolveContactCallback() const
{
	return m_contactListener.IsRunningPreSolveContactCallback();
}
