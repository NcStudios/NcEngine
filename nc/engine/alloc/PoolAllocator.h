// #pragma once

// #include <cstddef>

// namespace nc::engine::alloc
// {
//     struct Chunk
//     {
//         /**
//          */
//         Chunk * next;
//     };

//     class PoolAllocator
//     {
//         public:
//             PoolAllocator(const size_t itemSize, const size_t itemsPerBlock)
//                 : m_itemSize {itemSize}, m_itemsPerBlock{itemsPerBlock}
//             {
//             }

//             ~PoolAllocator()
//             {

//             }

//             void * allocate(size_t size);
//             void deallocate(void * chunk, size_t size);

//         private:
//             size_t m_itemSize;
//             size_t m_itemsPerBlock;
//             Chunk * m_block = nullptr;

//             Chunk * allocateBlock();
//     };

//     void * PoolAllocator::allocate(size_t size)
//     {
//         if (m_block == nullptr)
//         {
//             m_block = allocateBlock(size);
//         }
//     }

//     void PoolAllocator::deallocate(void * chunk, size_t size)
//     {

//     }

//     Chunk * PoolAllocator::allocateBlock();
// }