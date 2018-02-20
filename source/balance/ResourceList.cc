#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceList.hpp>
#include <coffee/balance/GuardResourceList.hpp>

#include <mutex>

#include <coffee/adt/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/balance/SCCS.hpp>

using namespace coffee;

void balance::ResourceList::initialize ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   SCCS::activate();

   GuardResourceList guard(m_mutex);

   for (resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      try {
         resource(ii)->initialize();
         LOG_DEBUG(resource(ii)->asString());
      }
      catch (adt::RuntimeException& ex) {
         LOG_ERROR(resource (ii)->getName () << " | " << ex.asString());
      }
   }

   if (m_resources.empty ())
      LOG_WARN (asString() << " does not have any resource");

   if (countAvailableResources(guard) == 0)
      LOG_WARN (asString() << " does not have any available resource");
}

bool balance::ResourceList::add (const std::shared_ptr<Resource>& resource)
   throw (adt::RuntimeException)
{
    logger::TraceMethod tm (logger::Level::Local7, COFFEE_FILE_LOCATION);

   if (!resource) {
      COFFEE_THROW_EXCEPTION(asString() << " can not add an empty resource");
   }

   bool result = true;

   if (true) {
      GuardResourceList guard(m_mutex);

      for (const_resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
         if (ResourceList::resource(ii)->getName() == resource->getName()) {
            result = false;
            break;
         }
      }

      if (result == true) {
         m_resources.push_back (resource);
      }
   }

   LOG_INFO (asString() << " | Add: " << resource->asString() << " | Result=" << adt::AsString::apply (result));

   return result;
}

size_t balance::ResourceList::countAvailableResources (balance::GuardResourceList& guard) const
   noexcept
{
   size_t result = 0;

   for (const_resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      if (resource(ii)->isAvailable())
         result ++;
   }

   return result;
}

balance::ResourceList::resource_iterator balance::ResourceList::next(balance::GuardResourceList& guard, resource_iterator ii)
   noexcept
{
   ii ++;

   if (ii == resource_end(guard))
      ii = resource_begin(guard);

   return ii;
}

//virtual
adt::StreamString balance::ResourceList::asString () const
   noexcept
{
   adt::StreamString result ("balance.ResourceList { ");
   result += adt::NamedObject::asString();
   result += " | Available = ";

   GuardResourceList fakeGuard(*this);
   result += adt::AsString::apply(countAvailableResources(fakeGuard));
   result.append (" of ").append (adt::AsString::apply (size (fakeGuard)));
   return result.append (" }");
}

//virtual
std::shared_ptr<xml::Node> balance::ResourceList::asXML (std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild(this->getName());

   GuardResourceList fakeGuard(*this);
   for (const_resource_iterator ii = resource_begin(fakeGuard), maxii = resource_end(fakeGuard); ii != maxii; ++ ii) {
      resource(ii)->asXML(result);
   }

   return result;
}
