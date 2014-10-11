
#include "kbase\path_service.h"

#include <forward_list>
#include <mutex>
#include <unordered_map>

#include "kbase\base_path_provider.h"
#include "kbase\error_exception_util.h"

using kbase::FilePath;
typedef kbase::PathService::PathKey PathKey;

namespace kbase {

FilePath BasePathProvider(PathKey);

}   // namespace kbase

namespace {

typedef kbase::PathService::ProviderFunc ProviderFunc;
typedef std::unordered_map<PathKey, FilePath> PathMap;

// Both |start| and |end| are used to prevent path keys claimed by different 
// providers being overlapped.
struct PathProvider {
    ProviderFunc fn;
    PathKey start;
    PathKey end;
};

// We keep providers in a linked list, and ensure that our provider is always placed
// in tail.
typedef std::forward_list<PathProvider> ProviderChain;

struct PathData {
    ProviderChain providers;
    std::mutex lock;
    PathMap cached_path_table;
    bool cache_disabled;

    PathData()
        : providers({ PathProvider { kbase::BasePathProvider,
                                     kbase::BASE_PATH_START,
                                     kbase::BASE_PATH_END }}),
          cache_disabled(false)
    {}
};

// TODO: replace with LazyInstance object
PathData g_path_data;

PathData& GetPathData()
{
    return g_path_data;
}

}   // namespace

namespace kbase {

// static
FilePath PathService::Get(PathKey key)
{
    PathData& path_data = GetPathData();
    ENSURE(key >= BASE_PATH_START)(key).raise();
    ENSURE(path_data.providers.empty() == false).raise();

    // Special case. We always use our provider for retrieving current directory and
    // never cache it.
    if (key == DIR_CURRENT) {
        auto get_base_provider = [&path_data]()->ProviderChain::iterator {
            auto it = path_data.providers.begin();
            for (; std::next(it) != path_data.providers.end(); ++it) {
              break;
            }
            return it;
        };

        return get_base_provider()->fn(key);
    }


    return FilePath();
}

}   // namespace kbase