#pragma once

#include "IHandleEntity.hpp"

namespace SDK
{
	class ICollideable;
	class IClientNetworkable;
	class IClientRenderable;
	class IClientEntity;

	class AngularImpulse;
	class IPhysicsShadowController;

	class IPhysicsObject
	{
	public:
		virtual ~IPhysicsObject(void) {}

		// returns true if this object is static/unmoveable
		// NOTE: returns false for objects that are not created static, but set EnableMotion(false);
		// Call IsMoveable() to find if the object is static OR has motion disabled
		virtual bool			IsStatic() const = 0;
		virtual bool			IsAsleep() const = 0;
		virtual bool			IsTrigger() const = 0;
		virtual bool			IsFluid() const = 0;		// fluids are special triggers with fluid controllers attached, they return true to IsTrigger() as well!
		virtual bool			IsHinged() const = 0;
		virtual bool			IsCollisionEnabled() const = 0;
		virtual bool			IsGravityEnabled() const = 0;
		virtual bool			IsDragEnabled() const = 0;
		virtual bool			IsMotionEnabled() const = 0;
		virtual bool			IsMoveable() const = 0;	 // legacy: IsMotionEnabled() && !IsStatic()
		virtual bool			IsAttachedToConstraint(bool bExternalOnly) const = 0;

		// Enable / disable collisions for this object
		virtual void			EnableCollisions(bool enable) = 0;
		// Enable / disable gravity for this object
		virtual void			EnableGravity(bool enable) = 0;
		// Enable / disable air friction / drag for this object
		virtual void			EnableDrag(bool enable) = 0;
		// Enable / disable motion (pin / unpin the object)
		virtual void			EnableMotion(bool enable) = 0;

		// Game can store data in each object (link back to game object)
		virtual void			SetGameData(void *pGameData) = 0;
		virtual void			*GetGameData(void) const = 0;
		// This flags word can be defined by the game as well
		virtual void			SetGameFlags(unsigned short userFlags) = 0;
		virtual unsigned short	GetGameFlags(void) const = 0;
		virtual void			SetGameIndex(unsigned short gameIndex) = 0;
		virtual unsigned short	GetGameIndex(void) const = 0;

		// setup various callbacks for this object
		virtual void			SetCallbackFlags(unsigned short callbackflags) = 0;
		// get the current callback state for this object
		virtual unsigned short	GetCallbackFlags(void) const = 0;

		// "wakes up" an object
		// NOTE: ALL OBJECTS ARE "Asleep" WHEN CREATED
		virtual void			Wake(void) = 0;
		virtual void			Sleep(void) = 0;
		// call this when the collision filter conditions change due to this 
		// object's state (e.g. changing solid type or collision group)
		virtual void			RecheckCollisionFilter() = 0;
		// NOTE: Contact points aren't updated when collision rules change, call this to force an update
		// UNDONE: Force this in RecheckCollisionFilter() ?
		virtual void			RecheckContactPoints() = 0;

		// mass accessors
		virtual void			SetMass(float mass) = 0;
		virtual float			GetMass(void) const = 0;
		// get 1/mass (it's cached)
		virtual float			GetInvMass(void) const = 0;
		virtual Vector			GetInertia(void) const = 0;
		virtual Vector			GetInvInertia(void) const = 0;
		virtual void			SetInertia(const Vector &inertia) = 0;

		virtual void			SetDamping(const float *speed, const float *rot) = 0;
		virtual void			GetDamping(float *speed, float *rot) const = 0;

		// coefficients are optional, pass either
		virtual void			SetDragCoefficient(float *pDrag, float *pAngularDrag) = 0;
		virtual void			SetBuoyancyRatio(float ratio) = 0;			// Override bouyancy

		// material index
		virtual int				GetMaterialIndex() const = 0;
		virtual void			SetMaterialIndex(int materialIndex) = 0;

		// contents bits
		virtual unsigned int	GetContents() const = 0;
		virtual void			SetContents(unsigned int contents) = 0;

		// Get the radius if this is a sphere object (zero if this is a polygonal mesh)
		virtual float			GetSphereRadius() const = 0;
		virtual float			GetEnergy() const = 0;
		virtual Vector			GetMassCenterLocalSpace() const = 0;

		// NOTE: This will teleport the object
		virtual void			SetPosition(const Vector &worldPosition, const QAngle &angles, bool isTeleport) = 0;
		virtual void			SetPositionMatrix(const matrix3x4_t&matrix, bool isTeleport) = 0;

		virtual void			GetPosition(Vector *worldPosition, QAngle *angles) const = 0;
		virtual void			GetPositionMatrix(matrix3x4_t *positionMatrix) const = 0;
		// force the velocity to a new value
		// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
		// local axes (just like pev->velocity, pev->avelocity)
		virtual void			SetVelocity(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;

		// like the above, but force the change into the simulator immediately
		virtual void			SetVelocityInstantaneous(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;

		// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
		// local axes (just like pev->velocity, pev->avelocity)
		virtual void			GetVelocity(Vector *velocity, AngularImpulse *angularVelocity) const = 0;

		// NOTE: These are velocities, not forces.  i.e. They will have the same effect regardless of
		// the object's mass or inertia
		virtual void			AddVelocity(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;
		// gets a velocity in the object's local frame of reference at a specific point
		virtual void			GetVelocityAtPoint(const Vector &worldPosition, Vector *pVelocity) const = 0;
		// gets the velocity actually moved by the object in the last simulation update
		virtual void			GetImplicitVelocity(Vector *velocity, AngularImpulse *angularVelocity) const = 0;
		// NOTE:	These are here for convenience, but you can do them yourself by using the matrix
		//			returned from GetPositionMatrix()
		// convenient coordinate system transformations (params - dest, src)
		virtual void			LocalToWorld(Vector *worldPosition, const Vector &localPosition) const = 0;
		virtual void			WorldToLocal(Vector *localPosition, const Vector &worldPosition) const = 0;

		// transforms a vector (no translation) from object-local to world space
		virtual void			LocalToWorldVector(Vector *worldVector, const Vector &localVector) const = 0;
		// transforms a vector (no translation) from world to object-local space
		virtual void			WorldToLocalVector(Vector *localVector, const Vector &worldVector) const = 0;

		// push on an object
		// force vector is direction & magnitude of impulse kg in / s
		virtual void			ApplyForceCenter(const Vector &forceVector) = 0;
		virtual void			ApplyForceOffset(const Vector &forceVector, const Vector &worldPosition) = 0;
		// apply torque impulse.  This will change the angular velocity on the object.
		// HL Axes, kg degrees / s
		virtual void			ApplyTorqueCenter(const AngularImpulse &torque) = 0;

		// Calculates the force/torque on the center of mass for an offset force impulse (pass output to ApplyForceCenter / ApplyTorqueCenter)
		virtual void			CalculateForceOffset(const Vector &forceVector, const Vector &worldPosition, Vector *centerForce, AngularImpulse *centerTorque) const = 0;
		// Calculates the linear/angular velocities on the center of mass for an offset force impulse (pass output to AddVelocity)
		virtual void			CalculateVelocityOffset(const Vector &forceVector, const Vector &worldPosition, Vector *centerVelocity, AngularImpulse *centerAngularVelocity) const = 0;
		// calculate drag scale
		virtual float			CalculateLinearDrag(const Vector &unitDirection) const = 0;
		virtual float			CalculateAngularDrag(const Vector &objectSpaceRotationAxis) const = 0;

		// returns true if the object is in contact with another object
		// if true, puts a point on the contact surface in contactPoint, and
		// a pointer to the object in contactObject
		// NOTE: You can pass NULL for either to avoid computations
		// BUGBUG: Use CreateFrictionSnapshot instead of this - this is a simple hack
		virtual bool			GetContactPoint(Vector *contactPoint, IPhysicsObject **contactObject) const = 0;

		// refactor this a bit - move some of this to IPhysicsShadowController
		virtual void			SetShadow(float maxSpeed, float maxAngularSpeed, bool allowPhysicsMovement, bool allowPhysicsRotation) = 0;
		virtual void			UpdateShadow(const Vector &targetPosition, const QAngle &targetAngles, bool tempDisableGravity, float timeOffset) = 0;

		// returns number of ticks since last Update() call
		virtual int				GetShadowPosition(Vector *position, QAngle *angles) const = 0;
		virtual IPhysicsShadowController *GetShadowController(void) const = 0;
		virtual void			RemoveShadowController() = 0;
		// applies the math of the shadow controller to this object.
		// for use in your own controllers
		// returns the new value of secondsToArrival with dt time elapsed
		//virtual float			ComputeShadowControl(const hlshadowcontrol_params_t &params, float secondsToArrival, float dt) = 0;


		//virtual const CPhysCollide	*GetCollide(void) const = 0;
		//virtual const char			*GetName() const = 0;

		//virtual void			BecomeTrigger() = 0;
		//virtual void			RemoveTrigger() = 0;

		//// sets the object to be hinged.  Fixed it place, but able to rotate around one axis.
		//virtual void			BecomeHinged(int localAxis) = 0;
		//// resets the object to original state
		//virtual void			RemoveHinged() = 0;

		//// used to iterate the contact points of an object
		//virtual IPhysicsFrictionSnapshot *CreateFrictionSnapshot() = 0;
		//virtual void DestroyFrictionSnapshot(IPhysicsFrictionSnapshot *pSnapshot) = 0;

		//// dumps info about the object to Msg()
		//virtual void			OutputDebugInfo() const = 0;

	};

	class C_BaseEntity
	{
	public:

		IPhysicsObject* CreateVPhysics(void)
		{
			typedef IPhysicsObject*(__thiscall* OriginalFn)(void*);
			return GetVFunc<OriginalFn>(this, 95)(this);
		}

		int VPhysicsUpdate(IPhysicsObject *pList)
		{
			typedef int(__thiscall* OriginalFn)(void*, IPhysicsObject *);
			return GetVFunc<OriginalFn>(this, 97)(this, pList);
		}

		//95	C_BaseEntity::CreateVPhysics(void)
	};
	class IClientThinkable;
	class IClientAlphaProperty;

	class IClientUnknown : public IHandleEntity {
	public:
		virtual ICollideable*              GetCollideable() = 0;
		virtual IClientNetworkable*        GetClientNetworkable() = 0;
		virtual IClientRenderable*         GetClientRenderable() = 0;
		virtual IClientEntity*             GetIClientEntity() = 0;
		virtual C_BaseEntity*              GetBaseEntity() = 0;
		virtual IClientThinkable*          GetClientThinkable() = 0;
		//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
		virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;

	};
}