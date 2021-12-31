#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#ifndef AMD_VULKAN_MEMORY_ALLOCATOR_HPP
#define AMD_VULKAN_MEMORY_ALLOCATOR_HPP

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

#if !defined(VMA_HPP_NAMESPACE)
#define VMA_HPP_NAMESPACE vma
#endif

#define VMA_HPP_NAMESPACE_STRING VULKAN_HPP_STRINGIFY(VMA_HPP_NAMESPACE)

namespace VMA_HPP_NAMESPACE {
  class Allocation;
  class Allocator;
  class DefragmentationContext;
  class Pool;

  struct AllocationCreateInfo;
  struct AllocationInfo;
  struct AllocatorCreateInfo;
  struct DefragmentationInfo2;
  struct DefragmentationStats;
  struct DeviceMemoryCallbacks;
  struct PoolCreateInfo;
  struct PoolStats;
  struct RecordSettings;
  struct StatInfo;
  struct Stats;
  struct Budget;
  struct VulkanFunctions;

  enum class MemoryUsage
  {
    eUnknown = VMA_MEMORY_USAGE_UNKNOWN,
    eGpuOnly = VMA_MEMORY_USAGE_GPU_ONLY,
    eCpuOnly = VMA_MEMORY_USAGE_CPU_ONLY,
    eCpuToGpu = VMA_MEMORY_USAGE_CPU_TO_GPU,
    eGpuToCpu = VMA_MEMORY_USAGE_GPU_TO_CPU,
    eCpuCopy = VMA_MEMORY_USAGE_CPU_COPY,
    eGpuLazilyAllocated = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED
  };

  VULKAN_HPP_INLINE std::string to_string( MemoryUsage value )
  {
    switch ( value )
    {
      case MemoryUsage::eUnknown : return "Unknown";
      case MemoryUsage::eGpuOnly : return "GpuOnly";
      case MemoryUsage::eCpuOnly : return "CpuOnly";
      case MemoryUsage::eCpuToGpu : return "CpuToGpu";
      case MemoryUsage::eGpuToCpu : return "GpuToCpu";
      case MemoryUsage::eCpuCopy : return "CpuCopy";
      case MemoryUsage::eGpuLazilyAllocated : return "GpuLazilyAllocated";
      default: return "invalid";
    }
  }

  enum class AllocationCreateFlagBits : VmaAllocationCreateFlags
  {
    eDedicatedMemory = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
    eNeverAllocate = VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT,
    eMapped = VMA_ALLOCATION_CREATE_MAPPED_BIT,
    eCanBecomeLost = VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT,
    eCanMakeOtherLost = VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT,
    eUserDataCopyString = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT,
    eUpperAddress = VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT,
    eDontBind = VMA_ALLOCATION_CREATE_DONT_BIND_BIT,
    eWithinBudget = VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
    eStrategyBestFit = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT,
    eStrategyWorstFit = VMA_ALLOCATION_CREATE_STRATEGY_WORST_FIT_BIT,
    eStrategyFirstFit = VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT,
    eStrategyMinMemory = VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT,
    eStrategyMinTime = VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT,
    eStrategyMinFragmentation = VMA_ALLOCATION_CREATE_STRATEGY_MIN_FRAGMENTATION_BIT,
    eStrategyMask = VMA_ALLOCATION_CREATE_STRATEGY_MASK
  };

  VULKAN_HPP_INLINE std::string to_string( AllocationCreateFlagBits value )
  {
    switch ( value )
    {
      case AllocationCreateFlagBits::eDedicatedMemory : return "DedicatedMemory";
      case AllocationCreateFlagBits::eNeverAllocate : return "NeverAllocate";
      case AllocationCreateFlagBits::eMapped : return "Mapped";
      case AllocationCreateFlagBits::eCanBecomeLost : return "CanBecomeLost";
      case AllocationCreateFlagBits::eCanMakeOtherLost : return "CanMakeOtherLost";
      case AllocationCreateFlagBits::eUserDataCopyString : return "UserDataCopyString";
      case AllocationCreateFlagBits::eUpperAddress : return "UpperAddress";
      case AllocationCreateFlagBits::eDontBind : return "DontBind";
      case AllocationCreateFlagBits::eWithinBudget : return "WithinBudget";
      case AllocationCreateFlagBits::eStrategyBestFit : return "StrategyBestFit";
      case AllocationCreateFlagBits::eStrategyWorstFit : return "StrategyWorstFit";
      case AllocationCreateFlagBits::eStrategyFirstFit : return "StrategyFirstFit";
      default: return "invalid";
    }
  }

  using AllocationCreateFlags = VULKAN_HPP_NAMESPACE::Flags<AllocationCreateFlagBits>;

  VULKAN_HPP_INLINE AllocationCreateFlags operator|( AllocationCreateFlagBits bit0, AllocationCreateFlagBits bit1 )
  {
    return AllocationCreateFlags( bit0 ) | bit1;
  }

  VULKAN_HPP_INLINE AllocationCreateFlags operator~( AllocationCreateFlagBits bits )
  {
    return ~( AllocationCreateFlags( bits ) );
  }

  VULKAN_HPP_INLINE std::string to_string( AllocationCreateFlags value  )
  {
    if ( !value ) return "{}";
    std::string result;

    if ( value & AllocationCreateFlagBits::eDedicatedMemory ) result += "DedicatedMemory | ";
    if ( value & AllocationCreateFlagBits::eNeverAllocate ) result += "NeverAllocate | ";
    if ( value & AllocationCreateFlagBits::eMapped ) result += "Mapped | ";
    if ( value & AllocationCreateFlagBits::eCanBecomeLost ) result += "CanBecomeLost | ";
    if ( value & AllocationCreateFlagBits::eCanMakeOtherLost ) result += "CanMakeOtherLost | ";
    if ( value & AllocationCreateFlagBits::eUserDataCopyString ) result += "UserDataCopyString | ";
    if ( value & AllocationCreateFlagBits::eUpperAddress ) result += "UpperAddress | ";
    if ( value & AllocationCreateFlagBits::eDontBind ) result += "DontBind | ";
    if ( value & AllocationCreateFlagBits::eWithinBudget ) result += "WithinBudget | ";
    if ( value & AllocationCreateFlagBits::eStrategyBestFit ) result += "StrategyBestFit | ";
    if ( value & AllocationCreateFlagBits::eStrategyWorstFit ) result += "StrategyWorstFit | ";
    if ( value & AllocationCreateFlagBits::eStrategyFirstFit ) result += "StrategyFirstFit | ";
    if ( value & AllocationCreateFlagBits::eStrategyMinMemory ) result += "StrategyMinMemory | ";
    if ( value & AllocationCreateFlagBits::eStrategyMinTime ) result += "StrategyMinTime | ";
    if ( value & AllocationCreateFlagBits::eStrategyMinFragmentation ) result += "StrategyMinFragmentation | ";
    return "{ " + result.substr(0, result.size() - 3) + " }";
  }

  enum class AllocatorCreateFlagBits : VmaAllocatorCreateFlags
  {
    eExternallySynchronized = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
    eKhrDedicatedAllocation = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT,
    eKhrBindMemory2 = VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT,
    eExtMemoryBudget = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
  };

  VULKAN_HPP_INLINE std::string to_string( AllocatorCreateFlagBits value )
  {
    switch ( value )
    {
      case AllocatorCreateFlagBits::eExternallySynchronized : return "ExternallySynchronized";
      case AllocatorCreateFlagBits::eKhrDedicatedAllocation : return "KhrDedicatedAllocation";
      case AllocatorCreateFlagBits::eKhrBindMemory2 : return "KhrBindMemory2";
      case AllocatorCreateFlagBits::eExtMemoryBudget : return "ExtMemoryBudget";
      default: return "invalid";
    }
  }

  using AllocatorCreateFlags = VULKAN_HPP_NAMESPACE::Flags<AllocatorCreateFlagBits>;

  VULKAN_HPP_INLINE AllocatorCreateFlags operator|( AllocatorCreateFlagBits bit0, AllocatorCreateFlagBits bit1 )
  {
    return AllocatorCreateFlags( bit0 ) | bit1;
  }

  VULKAN_HPP_INLINE AllocatorCreateFlags operator~( AllocatorCreateFlagBits bits )
  {
    return ~( AllocatorCreateFlags( bits ) );
  }

  VULKAN_HPP_INLINE std::string to_string( AllocatorCreateFlags value  )
  {
    if ( !value ) return "{}";
    std::string result;

    if ( value & AllocatorCreateFlagBits::eExternallySynchronized ) result += "ExternallySynchronized | ";
    if ( value & AllocatorCreateFlagBits::eKhrDedicatedAllocation ) result += "KhrDedicatedAllocation | ";
    if ( value & AllocatorCreateFlagBits::eKhrBindMemory2 ) result += "KhrBindMemory2 | ";
    if ( value & AllocatorCreateFlagBits::eExtMemoryBudget ) result += "ExtMemoryBudget | ";
    return "{ " + result.substr(0, result.size() - 3) + " }";
  }

  enum class DefragmentationFlagBits : VmaDefragmentationFlags
  {};

  VULKAN_HPP_INLINE std::string to_string( DefragmentationFlagBits )
  {
    return "(void)";
  }

  using DefragmentationFlags = VULKAN_HPP_NAMESPACE::Flags<DefragmentationFlagBits>;

  VULKAN_HPP_INLINE std::string to_string( DefragmentationFlags  )
  {
    return "{}";
  }

  enum class PoolCreateFlagBits : VmaPoolCreateFlags
  {
    eIgnoreBufferImageGranularity = VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT,
    eLinearAlgorithm = VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT,
    eBuddyAlgorithm = VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT,
    eAlgorithmMask = VMA_POOL_CREATE_ALGORITHM_MASK
  };

  VULKAN_HPP_INLINE std::string to_string( PoolCreateFlagBits value )
  {
    switch ( value )
    {
      case PoolCreateFlagBits::eIgnoreBufferImageGranularity : return "IgnoreBufferImageGranularity";
      case PoolCreateFlagBits::eLinearAlgorithm : return "LinearAlgorithm";
      case PoolCreateFlagBits::eBuddyAlgorithm : return "BuddyAlgorithm";
      case PoolCreateFlagBits::eAlgorithmMask : return "AlgorithmMask";
      default: return "invalid";
    }
  }

  using PoolCreateFlags = VULKAN_HPP_NAMESPACE::Flags<PoolCreateFlagBits>;

  VULKAN_HPP_INLINE PoolCreateFlags operator|( PoolCreateFlagBits bit0, PoolCreateFlagBits bit1 )
  {
    return PoolCreateFlags( bit0 ) | bit1;
  }

  VULKAN_HPP_INLINE PoolCreateFlags operator~( PoolCreateFlagBits bits )
  {
    return ~( PoolCreateFlags( bits ) );
  }

  VULKAN_HPP_INLINE std::string to_string( PoolCreateFlags value  )
  {
    if ( !value ) return "{}";
    std::string result;

    if ( value & PoolCreateFlagBits::eIgnoreBufferImageGranularity ) result += "IgnoreBufferImageGranularity | ";
    if ( value & PoolCreateFlagBits::eLinearAlgorithm ) result += "LinearAlgorithm | ";
    if ( value & PoolCreateFlagBits::eBuddyAlgorithm ) result += "BuddyAlgorithm | ";
    return "{ " + result.substr(0, result.size() - 3) + " }";
  }

  enum class RecordFlagBits : VmaRecordFlags
  {
    eFlushAfterCall = VMA_RECORD_FLUSH_AFTER_CALL_BIT
  };

  VULKAN_HPP_INLINE std::string to_string( RecordFlagBits value )
  {
    switch ( value )
    {
      case RecordFlagBits::eFlushAfterCall : return "FlushAfterCall";
      default: return "invalid";
    }
  }

  using RecordFlags = VULKAN_HPP_NAMESPACE::Flags<RecordFlagBits>;

  VULKAN_HPP_INLINE RecordFlags operator|( RecordFlagBits bit0, RecordFlagBits bit1 )
  {
    return RecordFlags( bit0 ) | bit1;
  }

  VULKAN_HPP_INLINE RecordFlags operator~( RecordFlagBits bits )
  {
    return ~( RecordFlags( bits ) );
  }

  VULKAN_HPP_INLINE std::string to_string( RecordFlags value  )
  {
    if ( !value ) return "{}";
    std::string result;

    if ( value & RecordFlagBits::eFlushAfterCall ) result += "FlushAfterCall | ";
    return "{ " + result.substr(0, result.size() - 3) + " }";
  }

  class Allocator
  {
  public:
    using CType = VmaAllocator;

  public:
    VULKAN_HPP_CONSTEXPR Allocator()
      : m_allocator(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_CONSTEXPR Allocator( std::nullptr_t )
      : m_allocator(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_TYPESAFE_EXPLICIT Allocator( VmaAllocator allocator )
      : m_allocator( allocator )
    {}

#if defined(VULKAN_HPP_TYPESAFE_CONVERSION)
    Allocator & operator=(VmaAllocator allocator)
    {
      m_allocator = allocator;
      return *this; 
    }
#endif

    Allocator & operator=( std::nullptr_t )
    {
      m_allocator = VK_NULL_HANDLE;
      return *this;
    }

    bool operator==( Allocator const & rhs ) const
    {
      return m_allocator == rhs.m_allocator;
    }

    bool operator!=(Allocator const & rhs ) const
    {
      return m_allocator != rhs.m_allocator;
    }

    bool operator<(Allocator const & rhs ) const
    {
      return m_allocator < rhs.m_allocator;
    }

    VULKAN_HPP_NAMESPACE::Result allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements* pVkMemoryRequirements, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result allocateMemoryForBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type allocateMemoryForBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result allocateMemoryForImage( VULKAN_HPP_NAMESPACE::Image image, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type allocateMemoryForImage( VULKAN_HPP_NAMESPACE::Image image, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements* pVkMemoryRequirements, const AllocationCreateInfo* pCreateInfo, size_t allocationCount, Allocation* pAllocations, AllocationInfo* pAllocationInfos ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    template<typename VectorAllocator = std::allocator<AllocationInfo>>
    typename VULKAN_HPP_NAMESPACE::ResultValueType<std::vector<AllocationInfo,VectorAllocator>>::type allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations ) const;
    template<typename VectorAllocator = std::allocator<AllocationInfo>>
    typename VULKAN_HPP_NAMESPACE::ResultValueType<std::vector<AllocationInfo,VectorAllocator>>::type allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations, Allocator const& vectorAllocator ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result bindBufferMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Buffer buffer ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type bindBufferMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Buffer buffer ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result bindBufferMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Buffer buffer, const void* pNext ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type bindBufferMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Buffer buffer, const void* pNext ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result bindImageMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Image image ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type bindImageMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Image image ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result bindImageMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Image image, const void* pNext ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type bindImageMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Image image, const void* pNext ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void calculateStats( Stats* pStats ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    Stats calculateStats() const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void getBudget( Budget* pBudget ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    Budget getBudget() const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result checkCorruption( uint32_t memoryTypeBits ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type checkCorruption( uint32_t memoryTypeBits ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result checkPoolCorruption( Pool pool ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type checkPoolCorruption( Pool pool ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    void getPoolName( Pool pool, const char** ppName ) const;
#else
    const char* getPoolName( Pool pool ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  void setPoolName( Pool pool, const char* pName ) const;
#else
  void setPoolName( Pool pool, const char* pName ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, VULKAN_HPP_NAMESPACE::Buffer* pBuffer, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<std::pair<VULKAN_HPP_NAMESPACE::Buffer, vma::Allocation>>::type createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo & bufferCreateInfo, const AllocationCreateInfo & allocationCreateInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, VULKAN_HPP_NAMESPACE::Image* pImage, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<std::pair<VULKAN_HPP_NAMESPACE::Image, vma::Allocation>>::type createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo & imageCreateInfo, const AllocationCreateInfo & allocationCreateInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void createLostAllocation( Allocation* pAllocation ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    Allocation createLostAllocation() const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result createPool( const PoolCreateInfo* pCreateInfo, Pool* pPool ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<Pool>::type createPool( const PoolCreateInfo & createInfo ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result defragmentationBegin( const DefragmentationInfo2* pInfo, DefragmentationStats* pStats, DefragmentationContext* pContext ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<DefragmentationContext>::type defragmentationBegin( const DefragmentationInfo2 & info, VULKAN_HPP_NAMESPACE::Optional<DefragmentationStats> stats = nullptr ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::Result defragmentationEnd( DefragmentationContext context ) const;
#else
    VULKAN_HPP_NAMESPACE::ResultValueType<void>::type defragmentationEnd( DefragmentationContext context ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void destroy() const;

    void destroyBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, Allocation allocation ) const;

    void destroyImage( VULKAN_HPP_NAMESPACE::Image image, Allocation allocation ) const;

    void destroyPool( Pool pool ) const;

    VULKAN_HPP_NAMESPACE::Result findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo & allocationCreateInfo ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo & bufferCreateInfo, const AllocationCreateInfo & allocationCreateInfo ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo & imageCreateInfo, const AllocationCreateInfo & allocationCreateInfo ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void flushAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const;

    void freeMemory( Allocation allocation ) const;

    void freeMemoryPages( size_t allocationCount, Allocation* pAllocations ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    void freeMemoryPages( VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void getAllocationInfo( Allocation allocation, AllocationInfo* pAllocationInfo ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    AllocationInfo getAllocationInfo( Allocation allocation ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void getMemoryTypeProperties( uint32_t memoryTypeIndex, VULKAN_HPP_NAMESPACE::MemoryPropertyFlags* pFlags ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::MemoryPropertyFlags getMemoryTypeProperties( uint32_t memoryTypeIndex ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void getPoolStats( Pool pool, PoolStats* pPoolStats ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    PoolStats getPoolStats( Pool pool ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void invalidateAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const;

    void makePoolAllocationsLost( Pool pool, size_t* pLostAllocationCount ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    size_t makePoolAllocationsLost( Pool pool ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_NAMESPACE::Result mapMemory( Allocation allocation, void** ppData ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_NAMESPACE::ResultValueType<void*>::type mapMemory( Allocation allocation ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void setAllocationUserData( Allocation allocation, void* pUserData ) const;
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    void setAllocationUserData( Allocation allocation ) const;
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    void setCurrentFrameIndex( uint32_t frameIndex ) const;

    VULKAN_HPP_NAMESPACE::Bool32 touchAllocation( Allocation allocation ) const;

    void unmapMemory( Allocation allocation ) const;

    VULKAN_HPP_TYPESAFE_EXPLICIT operator VmaAllocator() const
    {
      return m_allocator;
    }

    explicit operator bool() const
    {
      return m_allocator != VK_NULL_HANDLE;
    }

    bool operator!() const
    {
      return m_allocator == VK_NULL_HANDLE;
    }

  private:
    VmaAllocator m_allocator;
  };
  static_assert( sizeof( Allocator ) == sizeof( VmaAllocator ), "handle and wrapper have different size!" );

  class Pool
  {
  public:
    using CType = VmaPool;

  public:
    VULKAN_HPP_CONSTEXPR Pool()
      : m_pool(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_CONSTEXPR Pool( std::nullptr_t )
      : m_pool(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_TYPESAFE_EXPLICIT Pool( VmaPool pool )
      : m_pool( pool )
    {}

#if defined(VULKAN_HPP_TYPESAFE_CONVERSION)
    Pool & operator=(VmaPool pool)
    {
      m_pool = pool;
      return *this; 
    }
#endif

    Pool & operator=( std::nullptr_t )
    {
      m_pool = VK_NULL_HANDLE;
      return *this;
    }

    bool operator==( Pool const & rhs ) const
    {
      return m_pool == rhs.m_pool;
    }

    bool operator!=(Pool const & rhs ) const
    {
      return m_pool != rhs.m_pool;
    }

    bool operator<(Pool const & rhs ) const
    {
      return m_pool < rhs.m_pool;
    }

    VULKAN_HPP_TYPESAFE_EXPLICIT operator VmaPool() const
    {
      return m_pool;
    }

    explicit operator bool() const
    {
      return m_pool != VK_NULL_HANDLE;
    }

    bool operator!() const
    {
      return m_pool == VK_NULL_HANDLE;
    }

  private:
    VmaPool m_pool;
  };
  static_assert( sizeof( Pool ) == sizeof( VmaPool ), "handle and wrapper have different size!" );
   
  class Allocation
  {
  public:
    using CType = VmaAllocation;

  public:
    VULKAN_HPP_CONSTEXPR Allocation()
      : m_allocation(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_CONSTEXPR Allocation( std::nullptr_t )
      : m_allocation(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_TYPESAFE_EXPLICIT Allocation( VmaAllocation allocation )
      : m_allocation( allocation )
    {}

#if defined(VULKAN_HPP_TYPESAFE_CONVERSION)
    Allocation & operator=(VmaAllocation allocation)
    {
      m_allocation = allocation;
      return *this; 
    }
#endif

    Allocation & operator=( std::nullptr_t )
    {
      m_allocation = VK_NULL_HANDLE;
      return *this;
    }

    bool operator==( Allocation const & rhs ) const
    {
      return m_allocation == rhs.m_allocation;
    }

    bool operator!=(Allocation const & rhs ) const
    {
      return m_allocation != rhs.m_allocation;
    }

    bool operator<(Allocation const & rhs ) const
    {
      return m_allocation < rhs.m_allocation;
    }

    VULKAN_HPP_TYPESAFE_EXPLICIT operator VmaAllocation() const
    {
      return m_allocation;
    }

    explicit operator bool() const
    {
      return m_allocation != VK_NULL_HANDLE;
    }

    bool operator!() const
    {
      return m_allocation == VK_NULL_HANDLE;
    }

  private:
    VmaAllocation m_allocation;
  };
  static_assert( sizeof( Allocation ) == sizeof( VmaAllocation ), "handle and wrapper have different size!" );

  class DefragmentationContext
  {
  public:
    using CType = VmaDefragmentationContext;

  public:
    VULKAN_HPP_CONSTEXPR DefragmentationContext()
      : m_defragmentationContext(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_CONSTEXPR DefragmentationContext( std::nullptr_t )
      : m_defragmentationContext(VK_NULL_HANDLE)
    {}

    VULKAN_HPP_TYPESAFE_EXPLICIT DefragmentationContext( VmaDefragmentationContext defragmentationContext )
      : m_defragmentationContext( defragmentationContext )
    {}

#if defined(VULKAN_HPP_TYPESAFE_CONVERSION)
    DefragmentationContext & operator=(VmaDefragmentationContext defragmentationContext)
    {
      m_defragmentationContext = defragmentationContext;
      return *this; 
    }
#endif

    DefragmentationContext & operator=( std::nullptr_t )
    {
      m_defragmentationContext = VK_NULL_HANDLE;
      return *this;
    }

    bool operator==( DefragmentationContext const & rhs ) const
    {
      return m_defragmentationContext == rhs.m_defragmentationContext;
    }

    bool operator!=(DefragmentationContext const & rhs ) const
    {
      return m_defragmentationContext != rhs.m_defragmentationContext;
    }

    bool operator<(DefragmentationContext const & rhs ) const
    {
      return m_defragmentationContext < rhs.m_defragmentationContext;
    }

    VULKAN_HPP_TYPESAFE_EXPLICIT operator VmaDefragmentationContext() const
    {
      return m_defragmentationContext;
    }

    explicit operator bool() const
    {
      return m_defragmentationContext != VK_NULL_HANDLE;
    }

    bool operator!() const
    {
      return m_defragmentationContext == VK_NULL_HANDLE;
    }

  private:
    VmaDefragmentationContext m_defragmentationContext;
  };
  static_assert( sizeof( DefragmentationContext ) == sizeof( VmaDefragmentationContext ), "handle and wrapper have different size!" );

  VULKAN_HPP_NAMESPACE::Result createAllocator( const AllocatorCreateInfo* pCreateInfo, Allocator* pAllocator );
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_NAMESPACE::ResultValueType<Allocator>::type createAllocator( const AllocatorCreateInfo & createInfo );
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  struct AllocationCreateInfo
  {
    AllocationCreateInfo( AllocationCreateFlags flags_ = AllocationCreateFlags(),
                                  MemoryUsage usage_ = MemoryUsage::eUnknown,
                                  VULKAN_HPP_NAMESPACE::MemoryPropertyFlags requiredFlags_ = VULKAN_HPP_NAMESPACE::MemoryPropertyFlags(),
                                  VULKAN_HPP_NAMESPACE::MemoryPropertyFlags preferredFlags_ = VULKAN_HPP_NAMESPACE::MemoryPropertyFlags(),
                                  uint32_t memoryTypeBits_ = 0,
                                  Pool pool_ = Pool(),
                                  void* pUserData_ = nullptr )
      : flags( flags_ )
      , usage( usage_ )
      , requiredFlags( requiredFlags_ )
      , preferredFlags( preferredFlags_ )
      , memoryTypeBits( memoryTypeBits_ )
      , pool( pool_ )
      , pUserData( pUserData_ )
    {}

    AllocationCreateInfo( VmaAllocationCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocationCreateInfo*>(this) = rhs;
    }

    AllocationCreateInfo& operator=( VmaAllocationCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocationCreateInfo*>(this) = rhs;
      return *this;
    }

    AllocationCreateInfo & setFlags( AllocationCreateFlags flags_ )
    {
      flags = flags_;
      return *this;
    }

    AllocationCreateInfo & setUsage( MemoryUsage usage_ )
    {
      usage = usage_;
      return *this;
    }

    AllocationCreateInfo & setRequiredFlags( VULKAN_HPP_NAMESPACE::MemoryPropertyFlags requiredFlags_ )
    {
      requiredFlags = requiredFlags_;
      return *this;
    }

    AllocationCreateInfo & setPreferredFlags( VULKAN_HPP_NAMESPACE::MemoryPropertyFlags preferredFlags_ )
    {
      preferredFlags = preferredFlags_;
      return *this;
    }

    AllocationCreateInfo & setMemoryTypeBits( uint32_t memoryTypeBits_ )
    {
      memoryTypeBits = memoryTypeBits_;
      return *this;
    }

    AllocationCreateInfo & setPool( Pool pool_ )
    {
      pool = pool_;
      return *this;
    }

    AllocationCreateInfo & setPUserData( void* pUserData_ )
    {
      pUserData = pUserData_;
      return *this;
    }

    operator VmaAllocationCreateInfo const&() const
    {
      return *reinterpret_cast<const VmaAllocationCreateInfo*>( this );
    }

    operator VmaAllocationCreateInfo &()
    {
      return *reinterpret_cast<VmaAllocationCreateInfo*>( this );
    }

    bool operator==( AllocationCreateInfo const& rhs ) const
    {
      return ( flags == rhs.flags )
          && ( usage == rhs.usage )
          && ( requiredFlags == rhs.requiredFlags )
          && ( preferredFlags == rhs.preferredFlags )
          && ( memoryTypeBits == rhs.memoryTypeBits )
          && ( pool == rhs.pool )
          && ( pUserData == rhs.pUserData );
    }

    bool operator!=( AllocationCreateInfo const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    AllocationCreateFlags flags;
    MemoryUsage usage;
    VULKAN_HPP_NAMESPACE::MemoryPropertyFlags requiredFlags;
    VULKAN_HPP_NAMESPACE::MemoryPropertyFlags preferredFlags;
    uint32_t memoryTypeBits;
    Pool pool;
    void* pUserData;
  };
  static_assert( sizeof( AllocationCreateInfo ) == sizeof( VmaAllocationCreateInfo ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<AllocationCreateInfo>::value, "struct wrapper is not a standard layout!" );

  struct AllocationInfo
  {
    AllocationInfo( uint32_t memoryType_ = 0,
                            VULKAN_HPP_NAMESPACE::DeviceMemory deviceMemory_ = VULKAN_HPP_NAMESPACE::DeviceMemory(),
                            VULKAN_HPP_NAMESPACE::DeviceSize offset_ = 0,
                            VULKAN_HPP_NAMESPACE::DeviceSize size_ = 0,
                            void* pMappedData_ = nullptr,
                            void* pUserData_ = nullptr )
      : memoryType( memoryType_ )
      , deviceMemory( deviceMemory_ )
      , offset( offset_ )
      , size( size_ )
      , pMappedData( pMappedData_ )
      , pUserData( pUserData_ )
    {}

    AllocationInfo( VmaAllocationInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocationInfo*>(this) = rhs;
    }

    AllocationInfo& operator=( VmaAllocationInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocationInfo*>(this) = rhs;
      return *this;
    }

    AllocationInfo & setMemoryType( uint32_t memoryType_ )
    {
      memoryType = memoryType_;
      return *this;
    }

    AllocationInfo & setDeviceMemory( VULKAN_HPP_NAMESPACE::DeviceMemory deviceMemory_ )
    {
      deviceMemory = deviceMemory_;
      return *this;
    }

    AllocationInfo & setOffset( VULKAN_HPP_NAMESPACE::DeviceSize offset_ )
    {
      offset = offset_;
      return *this;
    }

    AllocationInfo & setSize( VULKAN_HPP_NAMESPACE::DeviceSize size_ )
    {
      size = size_;
      return *this;
    }

    AllocationInfo & setPMappedData( void* pMappedData_ )
    {
      pMappedData = pMappedData_;
      return *this;
    }

    AllocationInfo & setPUserData( void* pUserData_ )
    {
      pUserData = pUserData_;
      return *this;
    }

    operator VmaAllocationInfo const&() const
    {
      return *reinterpret_cast<const VmaAllocationInfo*>( this );
    }

    operator VmaAllocationInfo &()
    {
      return *reinterpret_cast<VmaAllocationInfo*>( this );
    }

    bool operator==( AllocationInfo const& rhs ) const
    {
      return ( memoryType == rhs.memoryType )
          && ( deviceMemory == rhs.deviceMemory )
          && ( offset == rhs.offset )
          && ( size == rhs.size )
          && ( pMappedData == rhs.pMappedData )
          && ( pUserData == rhs.pUserData );
    }

    bool operator!=( AllocationInfo const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    uint32_t memoryType;
    VULKAN_HPP_NAMESPACE::DeviceMemory deviceMemory;
    VULKAN_HPP_NAMESPACE::DeviceSize offset;
    VULKAN_HPP_NAMESPACE::DeviceSize size;
    void* pMappedData;
    void* pUserData;
  };
  static_assert( sizeof( AllocationInfo ) == sizeof( VmaAllocationInfo ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<AllocationInfo>::value, "struct wrapper is not a standard layout!" );

  struct DeviceMemoryCallbacks
  {
    DeviceMemoryCallbacks( PFN_vmaAllocateDeviceMemoryFunction pfnAllocate_ = nullptr,
                                   PFN_vmaFreeDeviceMemoryFunction pfnFree_ = nullptr )
      : pfnAllocate( pfnAllocate_ )
      , pfnFree( pfnFree_ )
    {}

    DeviceMemoryCallbacks( VmaDeviceMemoryCallbacks const & rhs )
    {
      *reinterpret_cast<VmaDeviceMemoryCallbacks*>(this) = rhs;
    }

    DeviceMemoryCallbacks& operator=( VmaDeviceMemoryCallbacks const & rhs )
    {
      *reinterpret_cast<VmaDeviceMemoryCallbacks*>(this) = rhs;
      return *this;
    }

    DeviceMemoryCallbacks & setPfnAllocate( PFN_vmaAllocateDeviceMemoryFunction pfnAllocate_ )
    {
      pfnAllocate = pfnAllocate_;
      return *this;
    }

    DeviceMemoryCallbacks & setPfnFree( PFN_vmaFreeDeviceMemoryFunction pfnFree_ )
    {
      pfnFree = pfnFree_;
      return *this;
    }

    operator VmaDeviceMemoryCallbacks const&() const
    {
      return *reinterpret_cast<const VmaDeviceMemoryCallbacks*>( this );
    }

    operator VmaDeviceMemoryCallbacks &()
    {
      return *reinterpret_cast<VmaDeviceMemoryCallbacks*>( this );
    }

    bool operator==( DeviceMemoryCallbacks const& rhs ) const
    {
      return ( pfnAllocate == rhs.pfnAllocate )
          && ( pfnFree == rhs.pfnFree );
    }

    bool operator!=( DeviceMemoryCallbacks const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    PFN_vmaAllocateDeviceMemoryFunction pfnAllocate;
    PFN_vmaFreeDeviceMemoryFunction pfnFree;
  };
  static_assert( sizeof( DeviceMemoryCallbacks ) == sizeof( VmaDeviceMemoryCallbacks ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<DeviceMemoryCallbacks>::value, "struct wrapper is not a standard layout!" );

  struct VulkanFunctions
  {
    VulkanFunctions( PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties_ = nullptr,
                             PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties_ = nullptr,
                             PFN_vkAllocateMemory vkAllocateMemory_ = nullptr,
                             PFN_vkFreeMemory vkFreeMemory_ = nullptr,
                             PFN_vkMapMemory vkMapMemory_ = nullptr,
                             PFN_vkUnmapMemory vkUnmapMemory_ = nullptr,
                             PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges_ = nullptr,
                             PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges_ = nullptr,
                             PFN_vkBindBufferMemory vkBindBufferMemory_ = nullptr,
                             PFN_vkBindImageMemory vkBindImageMemory_ = nullptr,
                             PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements_ = nullptr,
                             PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements_ = nullptr,
                             PFN_vkCreateBuffer vkCreateBuffer_ = nullptr,
                             PFN_vkDestroyBuffer vkDestroyBuffer_ = nullptr,
                             PFN_vkCreateImage vkCreateImage_ = nullptr,
                             PFN_vkDestroyImage vkDestroyImage_ = nullptr,
                             PFN_vkCmdCopyBuffer vkCmdCopyBuffer_ = nullptr
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
                           , PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR_ = nullptr,
                             PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR_ = nullptr
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
                           , PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR_ = nullptr,
                             PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR_ = nullptr
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
                           , PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR_ = nullptr
#endif
                             ) 
      : vkGetPhysicalDeviceProperties( vkGetPhysicalDeviceProperties_ )
      , vkGetPhysicalDeviceMemoryProperties( vkGetPhysicalDeviceMemoryProperties_ )
      , vkAllocateMemory( vkAllocateMemory_ )
      , vkFreeMemory( vkFreeMemory_ )
      , vkMapMemory( vkMapMemory_ )
      , vkUnmapMemory( vkUnmapMemory_ )
      , vkFlushMappedMemoryRanges( vkFlushMappedMemoryRanges_ )
      , vkInvalidateMappedMemoryRanges( vkInvalidateMappedMemoryRanges_ )
      , vkBindBufferMemory( vkBindBufferMemory_ )
      , vkBindImageMemory( vkBindImageMemory_ )
      , vkGetBufferMemoryRequirements( vkGetBufferMemoryRequirements_ )
      , vkGetImageMemoryRequirements( vkGetImageMemoryRequirements_ )
      , vkCreateBuffer( vkCreateBuffer_ )
      , vkDestroyBuffer( vkDestroyBuffer_ )
      , vkCreateImage( vkCreateImage_ )
      , vkDestroyImage( vkDestroyImage_ )
      , vkCmdCopyBuffer( vkCmdCopyBuffer_ )
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
      , vkGetBufferMemoryRequirements2KHR(vkGetBufferMemoryRequirements2KHR_)
      , vkGetImageMemoryRequirements2KHR(vkGetImageMemoryRequirements2KHR_)
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
      , vkBindBufferMemory2KHR(vkBindBufferMemory2KHR_)
      , vkBindImageMemory2KHR(vkBindImageMemory2KHR_)
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
      , vkGetPhysicalDeviceMemoryProperties2KHR(vkGetPhysicalDeviceMemoryProperties2KHR_)
#endif
    {}

    VulkanFunctions( VmaVulkanFunctions const & rhs )
    {
      *reinterpret_cast<VmaVulkanFunctions*>(this) = rhs;
    }

    VulkanFunctions& operator=( VmaVulkanFunctions const & rhs )
    {
      *reinterpret_cast<VmaVulkanFunctions*>(this) = rhs;
      return *this;
    }

    VulkanFunctions & setVkGetPhysicalDeviceProperties( PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties_ )
    {
      vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties_;
      return *this;
    }

    VulkanFunctions & setVkGetPhysicalDeviceMemoryProperties( PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties_ )
    {
      vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties_;
      return *this;
    }

    VulkanFunctions & setVkAllocateMemory( PFN_vkAllocateMemory vkAllocateMemory_ )
    {
      vkAllocateMemory = vkAllocateMemory_;
      return *this;
    }

    VulkanFunctions & setVkFreeMemory( PFN_vkFreeMemory vkFreeMemory_ )
    {
      vkFreeMemory = vkFreeMemory_;
      return *this;
    }

    VulkanFunctions & setVkMapMemory( PFN_vkMapMemory vkMapMemory_ )
    {
      vkMapMemory = vkMapMemory_;
      return *this;
    }

    VulkanFunctions & setVkUnmapMemory( PFN_vkUnmapMemory vkUnmapMemory_ )
    {
      vkUnmapMemory = vkUnmapMemory_;
      return *this;
    }

    VulkanFunctions & setVkFlushMappedMemoryRanges( PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges_ )
    {
      vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges_;
      return *this;
    }

    VulkanFunctions & setVkInvalidateMappedMemoryRanges( PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges_ )
    {
      vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges_;
      return *this;
    }

    VulkanFunctions & setVkBindBufferMemory( PFN_vkBindBufferMemory vkBindBufferMemory_ )
    {
      vkBindBufferMemory = vkBindBufferMemory_;
      return *this;
    }

    VulkanFunctions & setVkBindImageMemory( PFN_vkBindImageMemory vkBindImageMemory_ )
    {
      vkBindImageMemory = vkBindImageMemory_;
      return *this;
    }

    VulkanFunctions & setVkGetBufferMemoryRequirements( PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements_ )
    {
      vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements_;
      return *this;
    }

    VulkanFunctions & setVkGetImageMemoryRequirements( PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements_ )
    {
      vkGetImageMemoryRequirements = vkGetImageMemoryRequirements_;
      return *this;
    }

    VulkanFunctions & setVkCreateBuffer( PFN_vkCreateBuffer vkCreateBuffer_ )
    {
      vkCreateBuffer = vkCreateBuffer_;
      return *this;
    }

    VulkanFunctions & setVkDestroyBuffer( PFN_vkDestroyBuffer vkDestroyBuffer_ )
    {
      vkDestroyBuffer = vkDestroyBuffer_;
      return *this;
    }

    VulkanFunctions & setVkCreateImage( PFN_vkCreateImage vkCreateImage_ )
    {
      vkCreateImage = vkCreateImage_;
      return *this;
    }

    VulkanFunctions & setVkDestroyImage( PFN_vkDestroyImage vkDestroyImage_ )
    {
      vkDestroyImage = vkDestroyImage_;
      return *this;
    }

    VulkanFunctions & setVkCmdCopyBuffer( PFN_vkCmdCopyBuffer vkCmdCopyBuffer_ )
    {
      vkCmdCopyBuffer = vkCmdCopyBuffer_;
      return *this;
    }

#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
    VulkanFunctions & setVkGetBufferMemoryRequirements2KHR( PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR_ )
    {
      vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR_;
      return *this;
    }

    VulkanFunctions & setVkGetImageMemoryRequirements2KHR( PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR_ )
    {
      vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR_;
      return *this;
    }
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
    VulkanFunctions & setVkBindBufferMemory2KHR( PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR_ )
    {
      vkBindBufferMemory2KHR = vkBindBufferMemory2KHR_;
      return *this;
    }

    VulkanFunctions & setVkBindImageMemory2KHR( PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR_ )
    {
      vkBindImageMemory2KHR = vkBindImageMemory2KHR_;
      return *this;
    }
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
    VulkanFunctions & setVkGetPhysicalDeviceMemoryProperties2KHR( PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR_ )
    {
      vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR_;
      return *this;
    }
#endif

    operator VmaVulkanFunctions const&() const
    {
      return *reinterpret_cast<const VmaVulkanFunctions*>( this );
    }

    operator VmaVulkanFunctions &()
    {
      return *reinterpret_cast<VmaVulkanFunctions*>( this );
    }

    bool operator==( VulkanFunctions const& rhs ) const
    {
      return ( vkGetPhysicalDeviceProperties == rhs.vkGetPhysicalDeviceProperties )
          && ( vkGetPhysicalDeviceMemoryProperties == rhs.vkGetPhysicalDeviceMemoryProperties )
          && ( vkAllocateMemory == rhs.vkAllocateMemory )
          && ( vkFreeMemory == rhs.vkFreeMemory )
          && ( vkMapMemory == rhs.vkMapMemory )
          && ( vkUnmapMemory == rhs.vkUnmapMemory )
          && ( vkFlushMappedMemoryRanges == rhs.vkFlushMappedMemoryRanges )
          && ( vkInvalidateMappedMemoryRanges == rhs.vkInvalidateMappedMemoryRanges )
          && ( vkBindBufferMemory == rhs.vkBindBufferMemory )
          && ( vkBindImageMemory == rhs.vkBindImageMemory )
          && ( vkGetBufferMemoryRequirements == rhs.vkGetBufferMemoryRequirements )
          && ( vkGetImageMemoryRequirements == rhs.vkGetImageMemoryRequirements )
          && ( vkCreateBuffer == rhs.vkCreateBuffer )
          && ( vkDestroyBuffer == rhs.vkDestroyBuffer )
          && ( vkCreateImage == rhs.vkCreateImage )
          && ( vkDestroyImage == rhs.vkDestroyImage )
          && ( vkCmdCopyBuffer == rhs.vkCmdCopyBuffer )
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
          && ( vkGetBufferMemoryRequirements2KHR == rhs.vkGetBufferMemoryRequirements2KHR )
          && ( vkGetImageMemoryRequirements2KHR == rhs.vkGetImageMemoryRequirements2KHR )
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
          && ( vkBindBufferMemory2KHR == rhs.vkBindBufferMemory2KHR )
          && ( vkBindImageMemory2KHR == rhs.vkBindImageMemory2KHR )
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
          && ( vkGetPhysicalDeviceMemoryProperties2KHR == rhs.vkGetPhysicalDeviceMemoryProperties2KHR )
#endif
          ;
    }

    bool operator!=( VulkanFunctions const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
    PFN_vkAllocateMemory vkAllocateMemory;
    PFN_vkFreeMemory vkFreeMemory;
    PFN_vkMapMemory vkMapMemory;
    PFN_vkUnmapMemory vkUnmapMemory;
    PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
    PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
    PFN_vkBindBufferMemory vkBindBufferMemory;
    PFN_vkBindImageMemory vkBindImageMemory;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
    PFN_vkCreateBuffer vkCreateBuffer;
    PFN_vkDestroyBuffer vkDestroyBuffer;
    PFN_vkCreateImage vkCreateImage;
    PFN_vkDestroyImage vkDestroyImage;
    PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
    PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
    PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
    PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
    PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
    PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
#endif
  };
  static_assert( sizeof( VulkanFunctions ) == sizeof( VmaVulkanFunctions ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<VulkanFunctions>::value, "struct wrapper is not a standard layout!" );

  struct RecordSettings
  {
    RecordSettings( RecordFlags flags_ = RecordFlags(),
                            const char* pFilePath_ = nullptr )
      : flags( flags_ )
      , pFilePath( pFilePath_ )
    {}

    RecordSettings( VmaRecordSettings const & rhs )
    {
      *reinterpret_cast<VmaRecordSettings*>(this) = rhs;
    }

    RecordSettings& operator=( VmaRecordSettings const & rhs )
    {
      *reinterpret_cast<VmaRecordSettings*>(this) = rhs;
      return *this;
    }

    RecordSettings & setFlags( RecordFlags flags_ )
    {
      flags = flags_;
      return *this;
    }

    RecordSettings & setPFilePath( const char* pFilePath_ )
    {
      pFilePath = pFilePath_;
      return *this;
    }

    operator VmaRecordSettings const&() const
    {
      return *reinterpret_cast<const VmaRecordSettings*>( this );
    }

    operator VmaRecordSettings &()
    {
      return *reinterpret_cast<VmaRecordSettings*>( this );
    }

    bool operator==( RecordSettings const& rhs ) const
    {
      return ( flags == rhs.flags )
          && ( pFilePath == rhs.pFilePath );
    }

    bool operator!=( RecordSettings const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    RecordFlags flags;
    const char* pFilePath;
  };
  static_assert( sizeof( RecordSettings ) == sizeof( VmaRecordSettings ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<RecordSettings>::value, "struct wrapper is not a standard layout!" );

  struct AllocatorCreateInfo
  {
    AllocatorCreateInfo( AllocatorCreateFlags flags_ = AllocatorCreateFlags(),
                                 VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice_ = VULKAN_HPP_NAMESPACE::PhysicalDevice(),
                                 VULKAN_HPP_NAMESPACE::Device device_ = VULKAN_HPP_NAMESPACE::Device(),
                                 VULKAN_HPP_NAMESPACE::DeviceSize preferredLargeHeapBlockSize_ = 0,
                                 const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks_ = nullptr,
                                 const DeviceMemoryCallbacks* pDeviceMemoryCallbacks_ = nullptr,
                                 uint32_t frameInUseCount_ = 0,
                                 const VULKAN_HPP_NAMESPACE::DeviceSize* pHeapSizeLimit_ = nullptr,
                                 const VulkanFunctions* pVulkanFunctions_ = nullptr,
                                 const RecordSettings* pRecordSettings_ = nullptr,
                                 VULKAN_HPP_NAMESPACE::Instance instance_ = VULKAN_HPP_NAMESPACE::Instance(),
                                 uint32_t vulkanApiVersion_ = VK_API_VERSION_1_0 )
      : flags( flags_ )
      , physicalDevice( physicalDevice_ )
      , device( device_ )
      , preferredLargeHeapBlockSize( preferredLargeHeapBlockSize_ )
      , pAllocationCallbacks( pAllocationCallbacks_ )
      , pDeviceMemoryCallbacks( pDeviceMemoryCallbacks_ )
      , frameInUseCount( frameInUseCount_ )
      , pHeapSizeLimit( pHeapSizeLimit_ )
      , pVulkanFunctions( pVulkanFunctions_ )
      , pRecordSettings( pRecordSettings_ )
      , instance( instance_ )
      , vulkanApiVersion( vulkanApiVersion_ )
    {}

    AllocatorCreateInfo( VmaAllocatorCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocatorCreateInfo*>(this) = rhs;
    }

    AllocatorCreateInfo& operator=( VmaAllocatorCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaAllocatorCreateInfo*>(this) = rhs;
      return *this;
    }

    AllocatorCreateInfo & setFlags( AllocatorCreateFlags flags_ )
    {
      flags = flags_;
      return *this;
    }

    AllocatorCreateInfo & setPhysicalDevice( VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice_ )
    {
      physicalDevice = physicalDevice_;
      return *this;
    }

    AllocatorCreateInfo & setDevice( VULKAN_HPP_NAMESPACE::Device device_ )
    {
      device = device_;
      return *this;
    }

    AllocatorCreateInfo & setPreferredLargeHeapBlockSize( VULKAN_HPP_NAMESPACE::DeviceSize preferredLargeHeapBlockSize_ )
    {
      preferredLargeHeapBlockSize = preferredLargeHeapBlockSize_;
      return *this;
    }

    AllocatorCreateInfo & setPAllocationCallbacks( const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks_ )
    {
      pAllocationCallbacks = pAllocationCallbacks_;
      return *this;
    }

    AllocatorCreateInfo & setPDeviceMemoryCallbacks( const DeviceMemoryCallbacks* pDeviceMemoryCallbacks_ )
    {
      pDeviceMemoryCallbacks = pDeviceMemoryCallbacks_;
      return *this;
    }

    AllocatorCreateInfo & setFrameInUseCount( uint32_t frameInUseCount_ )
    {
      frameInUseCount = frameInUseCount_;
      return *this;
    }

    AllocatorCreateInfo & setPHeapSizeLimit( const VULKAN_HPP_NAMESPACE::DeviceSize* pHeapSizeLimit_ )
    {
      pHeapSizeLimit = pHeapSizeLimit_;
      return *this;
    }

    AllocatorCreateInfo & setPVulkanFunctions( const VulkanFunctions* pVulkanFunctions_ )
    {
      pVulkanFunctions = pVulkanFunctions_;
      return *this;
    }

    AllocatorCreateInfo & setPRecordSettings( const RecordSettings* pRecordSettings_ )
    {
      pRecordSettings = pRecordSettings_;
      return *this;
    }

    AllocatorCreateInfo & setInstance( VULKAN_HPP_NAMESPACE::Instance instance_ )
    {
      instance = instance_;
      return *this;
    }

    AllocatorCreateInfo & setVulkanApiVersion( uint32_t vulkanApiVersion_ )
    {
      vulkanApiVersion = vulkanApiVersion_;
      return *this;
    }

    operator VmaAllocatorCreateInfo const&() const
    {
      return *reinterpret_cast<const VmaAllocatorCreateInfo*>( this );
    }

    operator VmaAllocatorCreateInfo &()
    {
      return *reinterpret_cast<VmaAllocatorCreateInfo*>( this );
    }

    bool operator==( AllocatorCreateInfo const& rhs ) const
    {
      return ( flags == rhs.flags )
          && ( physicalDevice == rhs.physicalDevice )
          && ( device == rhs.device )
          && ( preferredLargeHeapBlockSize == rhs.preferredLargeHeapBlockSize )
          && ( pAllocationCallbacks == rhs.pAllocationCallbacks )
          && ( pDeviceMemoryCallbacks == rhs.pDeviceMemoryCallbacks )
          && ( frameInUseCount == rhs.frameInUseCount )
          && ( pHeapSizeLimit == rhs.pHeapSizeLimit )
          && ( pVulkanFunctions == rhs.pVulkanFunctions )
          && ( pRecordSettings == rhs.pRecordSettings )
          && ( instance == rhs.instance )
          && ( vulkanApiVersion == rhs.vulkanApiVersion );
    }

    bool operator!=( AllocatorCreateInfo const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    AllocatorCreateFlags flags;
    VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice;
    VULKAN_HPP_NAMESPACE::Device device;
    VULKAN_HPP_NAMESPACE::DeviceSize preferredLargeHeapBlockSize;
    const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks;
    const DeviceMemoryCallbacks* pDeviceMemoryCallbacks;
    uint32_t frameInUseCount;
    const VULKAN_HPP_NAMESPACE::DeviceSize* pHeapSizeLimit;
    const VulkanFunctions* pVulkanFunctions;
    const RecordSettings* pRecordSettings;
    VULKAN_HPP_NAMESPACE::Instance instance;
    uint32_t vulkanApiVersion;
  };
  static_assert( sizeof( AllocatorCreateInfo ) == sizeof( VmaAllocatorCreateInfo ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<AllocatorCreateInfo>::value, "struct wrapper is not a standard layout!" );

  struct DefragmentationInfo2
  {
    DefragmentationInfo2( DefragmentationFlags flags_ = DefragmentationFlags(),
                                  uint32_t allocationCount_ = 0,
                                  Allocation* pAllocations_ = nullptr,
                                  VULKAN_HPP_NAMESPACE::Bool32* pAllocationsChanged_ = nullptr,
                                  uint32_t poolCount_ = 0,
                                  Pool* pPools_ = nullptr,
                                  VULKAN_HPP_NAMESPACE::DeviceSize maxCpuBytesToMove_ = 0,
                                  uint32_t maxCpuAllocationsToMove_ = 0,
                                  VULKAN_HPP_NAMESPACE::DeviceSize maxGpuBytesToMove_ = 0,
                                  uint32_t maxGpuAllocationsToMove_ = 0,
                                  VULKAN_HPP_NAMESPACE::CommandBuffer commandBuffer_ = VULKAN_HPP_NAMESPACE::CommandBuffer() )
      : flags( flags_ )
      , allocationCount( allocationCount_ )
      , pAllocations( pAllocations_ )
      , pAllocationsChanged( pAllocationsChanged_ )
      , poolCount( poolCount_ )
      , pPools( pPools_ )
      , maxCpuBytesToMove( maxCpuBytesToMove_ )
      , maxCpuAllocationsToMove( maxCpuAllocationsToMove_ )
      , maxGpuBytesToMove( maxGpuBytesToMove_ )
      , maxGpuAllocationsToMove( maxGpuAllocationsToMove_ )
      , commandBuffer( commandBuffer_ )
    {}

    DefragmentationInfo2( VmaDefragmentationInfo2 const & rhs )
    {
      *reinterpret_cast<VmaDefragmentationInfo2*>(this) = rhs;
    }

    DefragmentationInfo2& operator=( VmaDefragmentationInfo2 const & rhs )
    {
      *reinterpret_cast<VmaDefragmentationInfo2*>(this) = rhs;
      return *this;
    }

    DefragmentationInfo2 & setFlags( DefragmentationFlags flags_ )
    {
      flags = flags_;
      return *this;
    }

    DefragmentationInfo2 & setAllocationCount( uint32_t allocationCount_ )
    {
      allocationCount = allocationCount_;
      return *this;
    }

    DefragmentationInfo2 & setPAllocations( Allocation* pAllocations_ )
    {
      pAllocations = pAllocations_;
      return *this;
    }

    DefragmentationInfo2 & setPAllocationsChanged( VULKAN_HPP_NAMESPACE::Bool32* pAllocationsChanged_ )
    {
      pAllocationsChanged = pAllocationsChanged_;
      return *this;
    }

    DefragmentationInfo2 & setPoolCount( uint32_t poolCount_ )
    {
      poolCount = poolCount_;
      return *this;
    }

    DefragmentationInfo2 & setPPools( Pool* pPools_ )
    {
      pPools = pPools_;
      return *this;
    }

    DefragmentationInfo2 & setMaxCpuBytesToMove( VULKAN_HPP_NAMESPACE::DeviceSize maxCpuBytesToMove_ )
    {
      maxCpuBytesToMove = maxCpuBytesToMove_;
      return *this;
    }

    DefragmentationInfo2 & setMaxCpuAllocationsToMove( uint32_t maxCpuAllocationsToMove_ )
    {
      maxCpuAllocationsToMove = maxCpuAllocationsToMove_;
      return *this;
    }

    DefragmentationInfo2 & setMaxGpuBytesToMove( VULKAN_HPP_NAMESPACE::DeviceSize maxGpuBytesToMove_ )
    {
      maxGpuBytesToMove = maxGpuBytesToMove_;
      return *this;
    }

    DefragmentationInfo2 & setMaxGpuAllocationsToMove( uint32_t maxGpuAllocationsToMove_ )
    {
      maxGpuAllocationsToMove = maxGpuAllocationsToMove_;
      return *this;
    }

    DefragmentationInfo2 & setCommandBuffer( VULKAN_HPP_NAMESPACE::CommandBuffer commandBuffer_ )
    {
      commandBuffer = commandBuffer_;
      return *this;
    }

    operator VmaDefragmentationInfo2 const&() const
    {
      return *reinterpret_cast<const VmaDefragmentationInfo2*>( this );
    }

    operator VmaDefragmentationInfo2 &()
    {
      return *reinterpret_cast<VmaDefragmentationInfo2*>( this );
    }

    bool operator==( DefragmentationInfo2 const& rhs ) const
    {
      return ( flags == rhs.flags )
          && ( allocationCount == rhs.allocationCount )
          && ( pAllocations == rhs.pAllocations )
          && ( pAllocationsChanged == rhs.pAllocationsChanged )
          && ( poolCount == rhs.poolCount )
          && ( pPools == rhs.pPools )
          && ( maxCpuBytesToMove == rhs.maxCpuBytesToMove )
          && ( maxCpuAllocationsToMove == rhs.maxCpuAllocationsToMove )
          && ( maxGpuBytesToMove == rhs.maxGpuBytesToMove )
          && ( maxGpuAllocationsToMove == rhs.maxGpuAllocationsToMove )
          && ( commandBuffer == rhs.commandBuffer );
    }

    bool operator!=( DefragmentationInfo2 const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    DefragmentationFlags flags;
    uint32_t allocationCount;
    Allocation* pAllocations;
    VULKAN_HPP_NAMESPACE::Bool32* pAllocationsChanged;
    uint32_t poolCount;
    Pool* pPools;
    VULKAN_HPP_NAMESPACE::DeviceSize maxCpuBytesToMove;
    uint32_t maxCpuAllocationsToMove;
    VULKAN_HPP_NAMESPACE::DeviceSize maxGpuBytesToMove;
    uint32_t maxGpuAllocationsToMove;
    VULKAN_HPP_NAMESPACE::CommandBuffer commandBuffer;
  };
  static_assert( sizeof( DefragmentationInfo2 ) == sizeof( VmaDefragmentationInfo2 ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<DefragmentationInfo2>::value, "struct wrapper is not a standard layout!" );

  struct DefragmentationStats
  {
    DefragmentationStats( VULKAN_HPP_NAMESPACE::DeviceSize bytesMoved_ = 0,
                                  VULKAN_HPP_NAMESPACE::DeviceSize bytesFreed_ = 0,
                                  uint32_t allocationsMoved_ = 0,
                                  uint32_t deviceMemoryBlocksFreed_ = 0 )
      : bytesMoved( bytesMoved_ )
      , bytesFreed( bytesFreed_ )
      , allocationsMoved( allocationsMoved_ )
      , deviceMemoryBlocksFreed( deviceMemoryBlocksFreed_ )
    {}

    DefragmentationStats( VmaDefragmentationStats const & rhs )
    {
      *reinterpret_cast<VmaDefragmentationStats*>(this) = rhs;
    }

    DefragmentationStats& operator=( VmaDefragmentationStats const & rhs )
    {
      *reinterpret_cast<VmaDefragmentationStats*>(this) = rhs;
      return *this;
    }

    DefragmentationStats & setBytesMoved( VULKAN_HPP_NAMESPACE::DeviceSize bytesMoved_ )
    {
      bytesMoved = bytesMoved_;
      return *this;
    }

    DefragmentationStats & setBytesFreed( VULKAN_HPP_NAMESPACE::DeviceSize bytesFreed_ )
    {
      bytesFreed = bytesFreed_;
      return *this;
    }

    DefragmentationStats & setAllocationsMoved( uint32_t allocationsMoved_ )
    {
      allocationsMoved = allocationsMoved_;
      return *this;
    }

    DefragmentationStats & setDeviceMemoryBlocksFreed( uint32_t deviceMemoryBlocksFreed_ )
    {
      deviceMemoryBlocksFreed = deviceMemoryBlocksFreed_;
      return *this;
    }

    operator VmaDefragmentationStats const&() const
    {
      return *reinterpret_cast<const VmaDefragmentationStats*>( this );
    }

    operator VmaDefragmentationStats &()
    {
      return *reinterpret_cast<VmaDefragmentationStats*>( this );
    }

    bool operator==( DefragmentationStats const& rhs ) const
    {
      return ( bytesMoved == rhs.bytesMoved )
          && ( bytesFreed == rhs.bytesFreed )
          && ( allocationsMoved == rhs.allocationsMoved )
          && ( deviceMemoryBlocksFreed == rhs.deviceMemoryBlocksFreed );
    }

    bool operator!=( DefragmentationStats const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    VULKAN_HPP_NAMESPACE::DeviceSize bytesMoved;
    VULKAN_HPP_NAMESPACE::DeviceSize bytesFreed;
    uint32_t allocationsMoved;
    uint32_t deviceMemoryBlocksFreed;
  };
  static_assert( sizeof( DefragmentationStats ) == sizeof( VmaDefragmentationStats ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<DefragmentationStats>::value, "struct wrapper is not a standard layout!" );

  struct PoolCreateInfo
  {
    PoolCreateInfo( uint32_t memoryTypeIndex_ = 0,
                            PoolCreateFlags flags_ = PoolCreateFlags(),
                            VULKAN_HPP_NAMESPACE::DeviceSize blockSize_ = 0,
                            size_t minBlockCount_ = 0,
                            size_t maxBlockCount_ = 0,
                            uint32_t frameInUseCount_ = 0 )
      : memoryTypeIndex( memoryTypeIndex_ )
      , flags( flags_ )
      , blockSize( blockSize_ )
      , minBlockCount( minBlockCount_ )
      , maxBlockCount( maxBlockCount_ )
      , frameInUseCount( frameInUseCount_ )
    {}

    PoolCreateInfo( VmaPoolCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaPoolCreateInfo*>(this) = rhs;
    }

    PoolCreateInfo& operator=( VmaPoolCreateInfo const & rhs )
    {
      *reinterpret_cast<VmaPoolCreateInfo*>(this) = rhs;
      return *this;
    }

    PoolCreateInfo & setMemoryTypeIndex( uint32_t memoryTypeIndex_ )
    {
      memoryTypeIndex = memoryTypeIndex_;
      return *this;
    }

    PoolCreateInfo & setFlags( PoolCreateFlags flags_ )
    {
      flags = flags_;
      return *this;
    }

    PoolCreateInfo & setBlockSize( VULKAN_HPP_NAMESPACE::DeviceSize blockSize_ )
    {
      blockSize = blockSize_;
      return *this;
    }

    PoolCreateInfo & setMinBlockCount( size_t minBlockCount_ )
    {
      minBlockCount = minBlockCount_;
      return *this;
    }

    PoolCreateInfo & setMaxBlockCount( size_t maxBlockCount_ )
    {
      maxBlockCount = maxBlockCount_;
      return *this;
    }

    PoolCreateInfo & setFrameInUseCount( uint32_t frameInUseCount_ )
    {
      frameInUseCount = frameInUseCount_;
      return *this;
    }

    operator VmaPoolCreateInfo const&() const
    {
      return *reinterpret_cast<const VmaPoolCreateInfo*>( this );
    }

    operator VmaPoolCreateInfo &()
    {
      return *reinterpret_cast<VmaPoolCreateInfo*>( this );
    }

    bool operator==( PoolCreateInfo const& rhs ) const
    {
      return ( memoryTypeIndex == rhs.memoryTypeIndex )
          && ( flags == rhs.flags )
          && ( blockSize == rhs.blockSize )
          && ( minBlockCount == rhs.minBlockCount )
          && ( maxBlockCount == rhs.maxBlockCount )
          && ( frameInUseCount == rhs.frameInUseCount );
    }

    bool operator!=( PoolCreateInfo const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    uint32_t memoryTypeIndex;
    PoolCreateFlags flags;
    VULKAN_HPP_NAMESPACE::DeviceSize blockSize;
    size_t minBlockCount;
    size_t maxBlockCount;
    uint32_t frameInUseCount;
  };
  static_assert( sizeof( PoolCreateInfo ) == sizeof( VmaPoolCreateInfo ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<PoolCreateInfo>::value, "struct wrapper is not a standard layout!" );

  struct PoolStats
  {
    PoolStats( VULKAN_HPP_NAMESPACE::DeviceSize size_ = 0,
                       VULKAN_HPP_NAMESPACE::DeviceSize unusedSize_ = 0,
                       size_t allocationCount_ = 0,
                       size_t unusedRangeCount_ = 0,
                       VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax_ = 0,
                       size_t blockCount_ = 0 )
      : size( size_ )
      , unusedSize( unusedSize_ )
      , allocationCount( allocationCount_ )
      , unusedRangeCount( unusedRangeCount_ )
      , unusedRangeSizeMax( unusedRangeSizeMax_ )
      , blockCount( blockCount_ )
    {}

    PoolStats( VmaPoolStats const & rhs )
    {
      *reinterpret_cast<VmaPoolStats*>(this) = rhs;
    }

    PoolStats& operator=( VmaPoolStats const & rhs )
    {
      *reinterpret_cast<VmaPoolStats*>(this) = rhs;
      return *this;
    }

    PoolStats & setSize( VULKAN_HPP_NAMESPACE::DeviceSize size_ )
    {
      size = size_;
      return *this;
    }

    PoolStats & setUnusedSize( VULKAN_HPP_NAMESPACE::DeviceSize unusedSize_ )
    {
      unusedSize = unusedSize_;
      return *this;
    }

    PoolStats & setAllocationCount( size_t allocationCount_ )
    {
      allocationCount = allocationCount_;
      return *this;
    }

    PoolStats & setUnusedRangeCount( size_t unusedRangeCount_ )
    {
      unusedRangeCount = unusedRangeCount_;
      return *this;
    }

    PoolStats & setUnusedRangeSizeMax( VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax_ )
    {
      unusedRangeSizeMax = unusedRangeSizeMax_;
      return *this;
    }

    PoolStats & setBlockCount( size_t blockCount_ )
    {
      blockCount = blockCount_;
      return *this;
    }

    operator VmaPoolStats const&() const
    {
      return *reinterpret_cast<const VmaPoolStats*>( this );
    }

    operator VmaPoolStats &()
    {
      return *reinterpret_cast<VmaPoolStats*>( this );
    }

    bool operator==( PoolStats const& rhs ) const
    {
      return ( size == rhs.size )
          && ( unusedSize == rhs.unusedSize )
          && ( allocationCount == rhs.allocationCount )
          && ( unusedRangeCount == rhs.unusedRangeCount )
          && ( unusedRangeSizeMax == rhs.unusedRangeSizeMax )
          && ( blockCount == rhs.blockCount );
    }

    bool operator!=( PoolStats const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    VULKAN_HPP_NAMESPACE::DeviceSize size;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedSize;
    size_t allocationCount;
    size_t unusedRangeCount;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax;
    size_t blockCount;
  };
  static_assert( sizeof( PoolStats ) == sizeof( VmaPoolStats ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<PoolStats>::value, "struct wrapper is not a standard layout!" );

  struct StatInfo
  {
    StatInfo( uint32_t blockCount_ = 0,
                      uint32_t allocationCount_ = 0,
                      uint32_t unusedRangeCount_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize usedBytes_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize unusedBytes_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMin_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeAvg_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMax_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMin_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeAvg_ = 0,
                      VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax_ = 0 )
      : blockCount( blockCount_ )
      , allocationCount( allocationCount_ )
      , unusedRangeCount( unusedRangeCount_ )
      , usedBytes( usedBytes_ )
      , unusedBytes( unusedBytes_ )
      , allocationSizeMin( allocationSizeMin_ )
      , allocationSizeAvg( allocationSizeAvg_ )
      , allocationSizeMax( allocationSizeMax_ )
      , unusedRangeSizeMin( unusedRangeSizeMin_ )
      , unusedRangeSizeAvg( unusedRangeSizeAvg_ )
      , unusedRangeSizeMax( unusedRangeSizeMax_ )
    {}

    StatInfo( VmaStatInfo const & rhs )
    {
      *reinterpret_cast<VmaStatInfo*>(this) = rhs;
    }

    StatInfo& operator=( VmaStatInfo const & rhs )
    {
      *reinterpret_cast<VmaStatInfo*>(this) = rhs;
      return *this;
    }

    StatInfo & setBlockCount( uint32_t blockCount_ )
    {
      blockCount = blockCount_;
      return *this;
    }

    StatInfo & setAllocationCount( uint32_t allocationCount_ )
    {
      allocationCount = allocationCount_;
      return *this;
    }

    StatInfo & setUnusedRangeCount( uint32_t unusedRangeCount_ )
    {
      unusedRangeCount = unusedRangeCount_;
      return *this;
    }

    StatInfo & setUsedBytes( VULKAN_HPP_NAMESPACE::DeviceSize usedBytes_ )
    {
      usedBytes = usedBytes_;
      return *this;
    }

    StatInfo & setUnusedBytes( VULKAN_HPP_NAMESPACE::DeviceSize unusedBytes_ )
    {
      unusedBytes = unusedBytes_;
      return *this;
    }

    StatInfo & setAllocationSizeMin( VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMin_ )
    {
      allocationSizeMin = allocationSizeMin_;
      return *this;
    }

    StatInfo & setAllocationSizeAvg( VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeAvg_ )
    {
      allocationSizeAvg = allocationSizeAvg_;
      return *this;
    }

    StatInfo & setAllocationSizeMax( VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMax_ )
    {
      allocationSizeMax = allocationSizeMax_;
      return *this;
    }

    StatInfo & setUnusedRangeSizeMin( VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMin_ )
    {
      unusedRangeSizeMin = unusedRangeSizeMin_;
      return *this;
    }

    StatInfo & setUnusedRangeSizeAvg( VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeAvg_ )
    {
      unusedRangeSizeAvg = unusedRangeSizeAvg_;
      return *this;
    }

    StatInfo & setUnusedRangeSizeMax( VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax_ )
    {
      unusedRangeSizeMax = unusedRangeSizeMax_;
      return *this;
    }

    operator VmaStatInfo const&() const
    {
      return *reinterpret_cast<const VmaStatInfo*>( this );
    }

    operator VmaStatInfo &()
    {
      return *reinterpret_cast<VmaStatInfo*>( this );
    }

    bool operator==( StatInfo const& rhs ) const
    {
      return ( blockCount == rhs.blockCount )
          && ( allocationCount == rhs.allocationCount )
          && ( unusedRangeCount == rhs.unusedRangeCount )
          && ( usedBytes == rhs.usedBytes )
          && ( unusedBytes == rhs.unusedBytes )
          && ( allocationSizeMin == rhs.allocationSizeMin )
          && ( allocationSizeAvg == rhs.allocationSizeAvg )
          && ( allocationSizeMax == rhs.allocationSizeMax )
          && ( unusedRangeSizeMin == rhs.unusedRangeSizeMin )
          && ( unusedRangeSizeAvg == rhs.unusedRangeSizeAvg )
          && ( unusedRangeSizeMax == rhs.unusedRangeSizeMax );
    }

    bool operator!=( StatInfo const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    uint32_t blockCount;
    uint32_t allocationCount;
    uint32_t unusedRangeCount;
    VULKAN_HPP_NAMESPACE::DeviceSize usedBytes;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedBytes;
    VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMin;
    VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeAvg;
    VULKAN_HPP_NAMESPACE::DeviceSize allocationSizeMax;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMin;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeAvg;
    VULKAN_HPP_NAMESPACE::DeviceSize unusedRangeSizeMax;
  };
  static_assert( sizeof( StatInfo ) == sizeof( VmaStatInfo ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<StatInfo>::value, "struct wrapper is not a standard layout!" );

  struct Stats
  {
    Stats( StatInfo memoryType_[VK_MAX_MEMORY_TYPES] = {},
                   StatInfo memoryHeap_[VK_MAX_MEMORY_HEAPS] = {},
                   StatInfo total_ = StatInfo() )
      : total( total_ )
    {
      memcpy(memoryType, memoryType_, VK_MAX_MEMORY_TYPES);
      memcpy(memoryHeap, memoryHeap_, VK_MAX_MEMORY_HEAPS);
    }

    Stats( VmaStats const & rhs )
    {
      *reinterpret_cast<VmaStats*>(this) = rhs;
    }

    Stats& operator=( VmaStats const & rhs )
    {
      *reinterpret_cast<VmaStats*>(this) = rhs;
      return *this;
    }

    Stats & setMemoryType( StatInfo memoryType_[VK_MAX_MEMORY_TYPES] )
    {
      memcpy(memoryType, memoryType_, VK_MAX_MEMORY_TYPES);
      return *this;
    }

    Stats & setMemoryHeap( StatInfo memoryHeap_[VK_MAX_MEMORY_HEAPS] )
    {
      memcpy(memoryHeap, memoryHeap_, VK_MAX_MEMORY_HEAPS);
      return *this;
    }

    Stats & setTotal( StatInfo total_ )
    {
      total = total_;
      return *this;
    }

    operator VmaStats const&() const
    {
      return *reinterpret_cast<const VmaStats*>( this );
    }

    operator VmaStats &()
    {
      return *reinterpret_cast<VmaStats*>( this );
    }

    bool operator==( Stats const& rhs ) const
    {
      return ( memcmp(memoryType, rhs.memoryType, sizeof(memoryType)) == 0 )
          && ( memcmp(memoryHeap, rhs.memoryHeap, sizeof(memoryHeap)) == 0 )
          && ( total == rhs.total );
    }

    bool operator!=( Stats const& rhs ) const
    {
      return !operator==( rhs );
    }

  public:
    StatInfo memoryType[VK_MAX_MEMORY_TYPES];
    StatInfo memoryHeap[VK_MAX_MEMORY_HEAPS];
    StatInfo total;
  };
  static_assert( sizeof( Stats ) == sizeof( VmaStats ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<Stats>::value, "struct wrapper is not a standard layout!" );
  
  struct Budget
  {
    Budget( VULKAN_HPP_NAMESPACE::DeviceSize blockBytes_ = {},
            VULKAN_HPP_NAMESPACE::DeviceSize allocationBytes_ = {},
            VULKAN_HPP_NAMESPACE::DeviceSize usage_ = {},
            VULKAN_HPP_NAMESPACE::DeviceSize budget_ = {} )
      : blockBytes( blockBytes_ )
      , allocationBytes( allocationBytes_ )
      , usage( usage_ )
      , budget( budget_ )
    {}

    Budget( VmaBudget const & rhs )
    {
      *reinterpret_cast<VmaBudget*>(this) = rhs;
    }

    Budget& operator=( VmaBudget const & rhs )
    {
      *reinterpret_cast<VmaBudget*>(this) = rhs;
      return *this;
    }
    
    Budget & setBlockBytes( VULKAN_HPP_NAMESPACE::DeviceSize blockBytes_ )
    {
      blockBytes = blockBytes_;
      return *this;
    }

    Budget & setAllocationBytes( VULKAN_HPP_NAMESPACE::DeviceSize allocationBytes_ )
    {
      allocationBytes = allocationBytes_;
      return *this;
    }

    Budget & setUsage( VULKAN_HPP_NAMESPACE::DeviceSize usage_ )
    {
      usage = usage_;
      return *this;
    }

    Budget & setBudget( VULKAN_HPP_NAMESPACE::DeviceSize budget_ )
    {
      budget = budget_;
      return *this;
    }    

    operator VmaBudget const&() const
    {
      return *reinterpret_cast<const VmaBudget*>( this );
    }

    operator VmaBudget &()
    {
      return *reinterpret_cast<VmaBudget*>( this );
    }

    bool operator==( Budget const& rhs ) const
    {
      return ( blockBytes == rhs.blockBytes )
          && ( allocationBytes == rhs.allocationBytes )
          && ( usage == rhs.usage )
          && ( budget == rhs.budget );
    }

    bool operator!=( Budget const& rhs ) const
    {
      return !operator==( rhs );
    }
  public:
      VULKAN_HPP_NAMESPACE::DeviceSize blockBytes;
      VULKAN_HPP_NAMESPACE::DeviceSize allocationBytes;
      VULKAN_HPP_NAMESPACE::DeviceSize usage;
      VULKAN_HPP_NAMESPACE::DeviceSize budget;
  };
  static_assert( sizeof( Budget ) == sizeof( VmaBudget ), "struct and wrapper have different size!" );
  static_assert( std::is_standard_layout<Budget>::value, "struct wrapper is not a standard layout!" );

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result createAllocator( const AllocatorCreateInfo* pCreateInfo, Allocator* pAllocator)
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateAllocator( reinterpret_cast<const VmaAllocatorCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaAllocator*>( pAllocator ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<Allocator>::type createAllocator( const AllocatorCreateInfo & createInfo )
  {
    Allocator allocator;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateAllocator( reinterpret_cast<const VmaAllocatorCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocator*>( &allocator ) ) );
    return createResultValue( result, allocator, VMA_HPP_NAMESPACE_STRING"::createAllocator" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements* pVkMemoryRequirements, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemory( m_allocator, reinterpret_cast<const VkMemoryRequirements*>( pVkMemoryRequirements ), reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaAllocation*>( pAllocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type Allocator::allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo ) const
  {
    Allocation allocation;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemory( m_allocator, reinterpret_cast<const VkMemoryRequirements*>( &vkMemoryRequirements ), reinterpret_cast<const VmaAllocationCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocation*>( &allocation ), reinterpret_cast<VmaAllocationInfo*>( static_cast<AllocationInfo*>( allocationInfo ) ) ) );
    return createResultValue( result, allocation, VMA_HPP_NAMESPACE_STRING"::Allocator::allocateMemory" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::allocateMemoryForBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryForBuffer( m_allocator, static_cast<VkBuffer>( buffer ), reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaAllocation*>( pAllocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type Allocator::allocateMemoryForBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo ) const
  {
    Allocation allocation;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryForBuffer( m_allocator, static_cast<VkBuffer>( buffer ), reinterpret_cast<const VmaAllocationCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocation*>( &allocation ), reinterpret_cast<VmaAllocationInfo*>( static_cast<AllocationInfo*>( allocationInfo ) ) ) );
    return createResultValue( result, allocation, VMA_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryForBuffer" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::allocateMemoryForImage( VULKAN_HPP_NAMESPACE::Image image, const AllocationCreateInfo* pCreateInfo, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryForImage( m_allocator, static_cast<VkImage>( image ), reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaAllocation*>( pAllocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<Allocation>::type Allocator::allocateMemoryForImage( VULKAN_HPP_NAMESPACE::Image image, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo ) const
  {
    Allocation allocation;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryForImage( m_allocator, static_cast<VkImage>( image ), reinterpret_cast<const VmaAllocationCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocation*>( &allocation ), reinterpret_cast<VmaAllocationInfo*>( static_cast<AllocationInfo*>( allocationInfo ) ) ) );
    return createResultValue( result, allocation, VMA_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryForImage" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements* pVkMemoryRequirements, const AllocationCreateInfo* pCreateInfo, size_t allocationCount, Allocation* pAllocations, AllocationInfo* pAllocationInfos ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryPages( m_allocator, reinterpret_cast<const VkMemoryRequirements*>( pVkMemoryRequirements ), reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ), allocationCount, reinterpret_cast<VmaAllocation*>( pAllocations ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfos ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  template<typename VectorAllocator>
  VULKAN_HPP_INLINE typename VULKAN_HPP_NAMESPACE::ResultValueType<std::vector<AllocationInfo,VectorAllocator>>::type Allocator::allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations ) const
  {
    std::vector<AllocationInfo,VectorAllocator> allocationInfos( allocations.size() );
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryPages( m_allocator, reinterpret_cast<const VkMemoryRequirements*>( &vkMemoryRequirements ), reinterpret_cast<const VmaAllocationCreateInfo*>( &createInfo ), allocations.size() , reinterpret_cast<VmaAllocation*>( allocations.data() ), reinterpret_cast<VmaAllocationInfo*>( allocationInfos.data() ) ) );
    return createResultValue( result, allocationInfos, VMA_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryPages" );
  }
  template<typename VectorAllocator>
  VULKAN_HPP_INLINE typename VULKAN_HPP_NAMESPACE::ResultValueType<std::vector<AllocationInfo,VectorAllocator>>::type Allocator::allocateMemoryPages( const VULKAN_HPP_NAMESPACE::MemoryRequirements & vkMemoryRequirements, const AllocationCreateInfo & createInfo, VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations, Allocator const& vectorAllocator ) const
  {
    std::vector<AllocationInfo,VectorAllocator> allocationInfos( allocations.size(), vectorAllocator );
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaAllocateMemoryPages( m_allocator, reinterpret_cast<const VkMemoryRequirements*>( &vkMemoryRequirements ), reinterpret_cast<const VmaAllocationCreateInfo*>( &createInfo ), allocations.size() , reinterpret_cast<VmaAllocation*>( allocations.data() ), reinterpret_cast<VmaAllocationInfo*>( allocationInfos.data() ) ) );
    return createResultValue( result, allocationInfos, VMA_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryPages" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::bindBufferMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Buffer buffer ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindBufferMemory( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkBuffer>( buffer ) ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::bindBufferMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Buffer buffer ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindBufferMemory( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkBuffer>( buffer ) ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::bindBufferMemory" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::bindBufferMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Buffer buffer, const void* pNext ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindBufferMemory2( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( allocationLocalOffset ), static_cast<VkBuffer>( buffer ), pNext ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::bindBufferMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Buffer buffer, const void* pNext ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindBufferMemory2( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( allocationLocalOffset ), static_cast<VkBuffer>( buffer ), pNext ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::bindBufferMemory2" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::bindImageMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Image image ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindImageMemory( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkImage>( image ) ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::bindImageMemory( Allocation allocation, VULKAN_HPP_NAMESPACE::Image image ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindImageMemory( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkImage>( image ) ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::bindImageMemory" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result bindImageMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Image image, const void* pNext ) const;
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindImageMemory2( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( allocationLocalOffset ), static_cast<VkImage>( image ), pNext ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::bindImageMemory2( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize allocationLocalOffset, VULKAN_HPP_NAMESPACE::Image image, const void* pNext ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaBindImageMemory2( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( allocationLocalOffset ), static_cast<VkImage>( image ), pNext ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::bindImageMemory2" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::calculateStats( Stats* pStats ) const
  {
    ::vmaCalculateStats( m_allocator, reinterpret_cast<VmaStats*>( pStats ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE Stats Allocator::calculateStats() const
  {
    Stats stats;
    ::vmaCalculateStats( m_allocator, reinterpret_cast<VmaStats*>( &stats ) );
    return stats;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::getBudget( Budget* pBudget ) const
  {
    ::vmaGetBudget( m_allocator, reinterpret_cast<VmaBudget*>( pBudget ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE Budget Allocator::getBudget() const
  {
    Budget budget;
    ::vmaGetBudget( m_allocator, reinterpret_cast<VmaBudget*>( &budget ) );
    return budget;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::checkCorruption( uint32_t memoryTypeBits ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCheckCorruption( m_allocator, memoryTypeBits ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::checkCorruption( uint32_t memoryTypeBits ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCheckCorruption( m_allocator, memoryTypeBits ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::checkCorruption" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::checkPoolCorruption( Pool pool ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCheckPoolCorruption( m_allocator, static_cast<VmaPool>( pool ) ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::checkPoolCorruption( Pool pool ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCheckPoolCorruption( m_allocator, static_cast<VmaPool>( pool ) ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::checkPoolCorruption" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::getPoolName( Pool pool, const char** ppName ) const
  {
    ::vmaGetPoolName( m_allocator, static_cast<VmaPool>( pool ), ppName );
  }
#else
  VULKAN_HPP_INLINE const char* Allocator::getPoolName( Pool pool ) const
  {
    const char* pName = nullptr;
    ::vmaGetPoolName( m_allocator, static_cast<VmaPool>( pool ), &pName );
    return pName;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::setPoolName( Pool pool, const char* pName ) const
  {
    ::vmaSetPoolName( m_allocator, static_cast<VmaPool>( pool ), pName );
  }
#else
  VULKAN_HPP_INLINE void Allocator::setPoolName( Pool pool, const char* pName ) const
  {
    ::vmaSetPoolName( m_allocator, static_cast<VmaPool>( pool ), pName );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, VULKAN_HPP_NAMESPACE::Buffer* pBuffer, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateBuffer( m_allocator, reinterpret_cast<const VkBufferCreateInfo*>( pBufferCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ), reinterpret_cast<VkBuffer*>( pBuffer ), reinterpret_cast<VmaAllocation*>( pAllocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<std::pair<VULKAN_HPP_NAMESPACE::Buffer, vma::Allocation>>::type Allocator::createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo & bufferCreateInfo, const AllocationCreateInfo & allocationCreateInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo ) const
  {
    VULKAN_HPP_NAMESPACE::Buffer buffer;
    vma::Allocation allocation;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateBuffer( m_allocator, reinterpret_cast<const VkBufferCreateInfo*>( &bufferCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( &allocationCreateInfo ), reinterpret_cast<VkBuffer*>( &buffer ), reinterpret_cast<VmaAllocation*>( &allocation ), reinterpret_cast<VmaAllocationInfo*>( static_cast<AllocationInfo*>( allocationInfo ) ) ) );
    std::pair<VULKAN_HPP_NAMESPACE::Buffer, vma::Allocation> pair = std::make_pair( buffer, allocation );
    return createResultValue( result, pair, VMA_HPP_NAMESPACE_STRING"::Allocator::createBuffer" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, VULKAN_HPP_NAMESPACE::Image* pImage, Allocation* pAllocation, AllocationInfo* pAllocationInfo ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateImage( m_allocator, reinterpret_cast<const VkImageCreateInfo*>( pImageCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ), reinterpret_cast<VkImage*>( pImage ), reinterpret_cast<VmaAllocation*>( pAllocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<std::pair<VULKAN_HPP_NAMESPACE::Image, vma::Allocation>>::type Allocator::createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo & imageCreateInfo, const AllocationCreateInfo & allocationCreateInfo, VULKAN_HPP_NAMESPACE::Optional<AllocationInfo> allocationInfo ) const
  {
    VULKAN_HPP_NAMESPACE::Image image;
    vma::Allocation allocation;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreateImage( m_allocator, reinterpret_cast<const VkImageCreateInfo*>( &imageCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( &allocationCreateInfo ), reinterpret_cast<VkImage*>( &image ), reinterpret_cast<VmaAllocation*>( &allocation ), reinterpret_cast<VmaAllocationInfo*>( static_cast<AllocationInfo*>( allocationInfo ) ) ) );
    std::pair<VULKAN_HPP_NAMESPACE::Image, vma::Allocation> pair = std::make_pair( image, allocation );
    return createResultValue( result, pair, VMA_HPP_NAMESPACE_STRING"::Allocator::createImage" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::createLostAllocation( Allocation* pAllocation ) const
  {
    ::vmaCreateLostAllocation( m_allocator, reinterpret_cast<VmaAllocation*>( pAllocation ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE Allocation Allocator::createLostAllocation() const
  {
    Allocation allocation;
    ::vmaCreateLostAllocation( m_allocator, reinterpret_cast<VmaAllocation*>( &allocation ) );
    return allocation;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::createPool( const PoolCreateInfo* pCreateInfo, Pool* pPool ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreatePool( m_allocator, reinterpret_cast<const VmaPoolCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaPool*>( pPool ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<Pool>::type Allocator::createPool( const PoolCreateInfo & createInfo ) const
  {
    Pool pool;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaCreatePool( m_allocator, reinterpret_cast<const VmaPoolCreateInfo*>( &createInfo ), reinterpret_cast<VmaPool*>( &pool ) ) );
    return createResultValue( result, pool, VMA_HPP_NAMESPACE_STRING"::Allocator::createPool" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::defragmentationBegin( const DefragmentationInfo2* pInfo, DefragmentationStats* pStats, DefragmentationContext* pContext ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaDefragmentationBegin( m_allocator, reinterpret_cast<const VmaDefragmentationInfo2*>( pInfo ), reinterpret_cast<VmaDefragmentationStats*>( pStats ), reinterpret_cast<VmaDefragmentationContext*>( pContext ) ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<DefragmentationContext>::type Allocator::defragmentationBegin( const DefragmentationInfo2 & info, VULKAN_HPP_NAMESPACE::Optional<DefragmentationStats> stats ) const
  {
    DefragmentationContext context;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaDefragmentationBegin( m_allocator, reinterpret_cast<const VmaDefragmentationInfo2*>( &info ), reinterpret_cast<VmaDefragmentationStats*>( static_cast<DefragmentationStats*>( stats ) ), reinterpret_cast<VmaDefragmentationContext*>( &context ) ) );
    return createResultValue( result, context, VMA_HPP_NAMESPACE_STRING"::Allocator::defragmentationBegin" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::defragmentationEnd( DefragmentationContext context ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaDefragmentationEnd( m_allocator, static_cast<VmaDefragmentationContext>( context ) ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void>::type Allocator::defragmentationEnd( DefragmentationContext context ) const
  {
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaDefragmentationEnd( m_allocator, static_cast<VmaDefragmentationContext>( context ) ) );
    return createResultValue( result, VMA_HPP_NAMESPACE_STRING"::Allocator::defragmentationEnd" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::destroy() const
  {
    ::vmaDestroyAllocator( m_allocator );
  }
#else
  VULKAN_HPP_INLINE void Allocator::destroy() const
  {
    ::vmaDestroyAllocator( m_allocator );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::destroyBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, Allocation allocation ) const
  {
    ::vmaDestroyBuffer( m_allocator, static_cast<VkBuffer>( buffer ), static_cast<VmaAllocation>( allocation ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::destroyBuffer( VULKAN_HPP_NAMESPACE::Buffer buffer, Allocation allocation ) const
  {
    ::vmaDestroyBuffer( m_allocator, static_cast<VkBuffer>( buffer ), static_cast<VmaAllocation>( allocation ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::destroyImage( VULKAN_HPP_NAMESPACE::Image image, Allocation allocation ) const
  {
    ::vmaDestroyImage( m_allocator, static_cast<VkImage>( image ), static_cast<VmaAllocation>( allocation ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::destroyImage( VULKAN_HPP_NAMESPACE::Image image, Allocation allocation ) const
  {
    ::vmaDestroyImage( m_allocator, static_cast<VkImage>( image ), static_cast<VmaAllocation>( allocation ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::destroyPool( Pool pool ) const
  {
    ::vmaDestroyPool( m_allocator, static_cast<VmaPool>( pool ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::destroyPool( Pool pool ) const
  {
    ::vmaDestroyPool( m_allocator, static_cast<VmaPool>( pool ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndex( m_allocator, memoryTypeBits, reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ), pMemoryTypeIndex ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type Allocator::findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo & allocationCreateInfo ) const
  {
    uint32_t memoryTypeIndex;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndex( m_allocator, memoryTypeBits, reinterpret_cast<const VmaAllocationCreateInfo*>( &allocationCreateInfo ), &memoryTypeIndex ) );
    return createResultValue( result, memoryTypeIndex, VMA_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndex" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndexForBufferInfo( m_allocator, reinterpret_cast<const VkBufferCreateInfo*>( pBufferCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ), pMemoryTypeIndex ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type Allocator::findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo & bufferCreateInfo, const AllocationCreateInfo & allocationCreateInfo ) const
  {
    uint32_t memoryTypeIndex;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndexForBufferInfo( m_allocator, reinterpret_cast<const VkBufferCreateInfo*>( &bufferCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( &allocationCreateInfo ), &memoryTypeIndex ) );
    return createResultValue( result, memoryTypeIndex, VMA_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndexForBufferInfo" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndexForImageInfo( m_allocator, reinterpret_cast<const VkImageCreateInfo*>( pImageCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ), pMemoryTypeIndex ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<uint32_t>::type Allocator::findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo & imageCreateInfo, const AllocationCreateInfo & allocationCreateInfo ) const
  {
    uint32_t memoryTypeIndex;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaFindMemoryTypeIndexForImageInfo( m_allocator, reinterpret_cast<const VkImageCreateInfo*>( &imageCreateInfo ), reinterpret_cast<const VmaAllocationCreateInfo*>( &allocationCreateInfo ), &memoryTypeIndex ) );
    return createResultValue( result, memoryTypeIndex, VMA_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndexForImageInfo" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::flushAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const
  {
    ::vmaFlushAllocation( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( offset ), static_cast<VkDeviceSize>( size ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::flushAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const
  {
    ::vmaFlushAllocation( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( offset ), static_cast<VkDeviceSize>( size ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::freeMemory( Allocation allocation ) const
  {
    ::vmaFreeMemory( m_allocator, static_cast<VmaAllocation>( allocation ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::freeMemory( Allocation allocation ) const
  {
    ::vmaFreeMemory( m_allocator, static_cast<VmaAllocation>( allocation ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::freeMemoryPages( size_t allocationCount, Allocation* pAllocations ) const
  {
    ::vmaFreeMemoryPages( m_allocator, allocationCount, reinterpret_cast<VmaAllocation*>( pAllocations ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::freeMemoryPages( VULKAN_HPP_NAMESPACE::ArrayProxy<Allocation> allocations ) const
  {
    ::vmaFreeMemoryPages( m_allocator, allocations.size() , reinterpret_cast<VmaAllocation*>( allocations.data() ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::getAllocationInfo( Allocation allocation, AllocationInfo* pAllocationInfo ) const
  {
    ::vmaGetAllocationInfo( m_allocator, static_cast<VmaAllocation>( allocation ), reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE AllocationInfo Allocator::getAllocationInfo( Allocation allocation ) const
  {
    AllocationInfo allocationInfo;
    ::vmaGetAllocationInfo( m_allocator, static_cast<VmaAllocation>( allocation ), reinterpret_cast<VmaAllocationInfo*>( &allocationInfo ) );
    return allocationInfo;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::getMemoryTypeProperties( uint32_t memoryTypeIndex, VULKAN_HPP_NAMESPACE::MemoryPropertyFlags* pFlags ) const
  {
    ::vmaGetMemoryTypeProperties( m_allocator, memoryTypeIndex, reinterpret_cast<VkMemoryPropertyFlags*>( pFlags ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::MemoryPropertyFlags Allocator::getMemoryTypeProperties( uint32_t memoryTypeIndex ) const
  {
    VULKAN_HPP_NAMESPACE::MemoryPropertyFlags flags;
    ::vmaGetMemoryTypeProperties( m_allocator, memoryTypeIndex, reinterpret_cast<VkMemoryPropertyFlags*>( &flags ) );
    return flags;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::getPoolStats( Pool pool, PoolStats* pPoolStats ) const
  {
    ::vmaGetPoolStats( m_allocator, static_cast<VmaPool>( pool ), reinterpret_cast<VmaPoolStats*>( pPoolStats ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE PoolStats Allocator::getPoolStats( Pool pool ) const
  {
    PoolStats poolStats;
    ::vmaGetPoolStats( m_allocator, static_cast<VmaPool>( pool ), reinterpret_cast<VmaPoolStats*>( &poolStats ) );
    return poolStats;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::invalidateAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const
  {
    ::vmaInvalidateAllocation( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( offset ), static_cast<VkDeviceSize>( size ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::invalidateAllocation( Allocation allocation, VULKAN_HPP_NAMESPACE::DeviceSize offset, VULKAN_HPP_NAMESPACE::DeviceSize size ) const
  {
    ::vmaInvalidateAllocation( m_allocator, static_cast<VmaAllocation>( allocation ), static_cast<VkDeviceSize>( offset ), static_cast<VkDeviceSize>( size ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE void Allocator::makePoolAllocationsLost( Pool pool, size_t* pLostAllocationCount ) const
  {
    ::vmaMakePoolAllocationsLost( m_allocator, static_cast<VmaPool>( pool ), pLostAllocationCount );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE size_t Allocator::makePoolAllocationsLost( Pool pool ) const
  {
    size_t lostAllocationCount;
    ::vmaMakePoolAllocationsLost( m_allocator, static_cast<VmaPool>( pool ), &lostAllocationCount );
    return lostAllocationCount;
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Result Allocator::mapMemory( Allocation allocation, void** ppData ) const
  {
    return static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaMapMemory( m_allocator, static_cast<VmaAllocation>( allocation ), ppData ) );
  }
#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::ResultValueType<void*>::type Allocator::mapMemory( Allocation allocation ) const
  {
    void* pData;
    VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( ::vmaMapMemory( m_allocator, static_cast<VmaAllocation>( allocation ), &pData ) );
    return createResultValue( result, pData, VMA_HPP_NAMESPACE_STRING"::Allocator::mapMemory" );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::setAllocationUserData( Allocation allocation, void* pUserData ) const
  {
    ::vmaSetAllocationUserData( m_allocator, static_cast<VmaAllocation>( allocation ), pUserData );
  }
#else
  VULKAN_HPP_INLINE void Allocator::setAllocationUserData( Allocation allocation, void* pUserData ) const
  {
    ::vmaSetAllocationUserData( m_allocator, static_cast<VmaAllocation>( allocation ), pUserData );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::setCurrentFrameIndex( uint32_t frameIndex ) const
  {
    ::vmaSetCurrentFrameIndex( m_allocator, frameIndex );
  }
#else
  VULKAN_HPP_INLINE void Allocator::setCurrentFrameIndex( uint32_t frameIndex ) const
  {
    ::vmaSetCurrentFrameIndex( m_allocator, frameIndex );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Bool32 Allocator::touchAllocation( Allocation allocation ) const
  {
    return static_cast<Bool32>( ::vmaTouchAllocation( m_allocator, static_cast<VmaAllocation>( allocation ) ) );
  }
#else
  VULKAN_HPP_INLINE VULKAN_HPP_NAMESPACE::Bool32 Allocator::touchAllocation( Allocation allocation ) const
  {
    return ::vmaTouchAllocation( m_allocator, static_cast<VmaAllocation>( allocation ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

#ifdef VULKAN_HPP_DISABLE_ENHANCED_MODE
  VULKAN_HPP_INLINE void Allocator::unmapMemory( Allocation allocation ) const
  {
    ::vmaUnmapMemory( m_allocator, static_cast<VmaAllocation>( allocation ) );
  }
#else
  VULKAN_HPP_INLINE void Allocator::unmapMemory( Allocation allocation ) const
  {
    ::vmaUnmapMemory( m_allocator, static_cast<VmaAllocation>( allocation ) );
  }
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/
}

#endif
#if __GNUC__
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif
