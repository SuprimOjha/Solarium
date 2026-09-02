#include "solarium/physics/verlet.hpp"

namespace solarium::physics {
    void VelocityVertlet::updatePosition(
        celestial::CelestialBody& body,
        double detaTime
    )
    {
        const math::Vec3 position = body.position();
        const math::Vec3 velocity = body.velocity();
        const math::Vec3 acceleration = body.acceleration();
        const math::Vec3 newPosition = position + velocity * deltaTime + acceleration * ( 0.5 * deltaTime * deltaTime );
        
        body.setPosition(
            newPosition 
        );
      }

      void VelocityVerlet::updateVelocity(
        celestial::CelestialBody& body,
        const math::Vec3& newAcceleration,
        double deltaTime
          ){
            const math::Vec3 velocity = body.velocity();
            const math::Vec3 oldAcceleration = body.acceleration();
            const math::Vec3 newVelocity = velocity + ( oldAcceleration + newAcceleration ) * (0.5 * deltaTime);
            body.setVelocity(
                newVelocity
            );
            body.setAcceleration(
                newAcceleration
            );
          }
}