/**
 * @file ObjectPool.h
 * @brief Generic object pool for efficient memory management
 *
 * Implements the Object Pool pattern to reuse objects instead of
 * constantly allocating and deallocating memory
 */

#pragma once
#include <functional>
#include <memory>
#include <vector>

/**
 * @brief Generic Object Pool template class
 * @tparam T Type of objects to pool
 */
template <typename T>
class ObjectPool {
   private:
    std::vector<std::unique_ptr<T>> mPool;
    std::vector<T*> mAvailable;
    std::vector<T*> mInUse;

    std::function<std::unique_ptr<T>()> mFactory;
    std::function<void(T*)> mReset;

    size_t mPoolSize;
    bool mAllowGrowth;

   public:
    /**
     * @brief Constructor
     * @param poolSize Initial pool size
     * @param factory Function to create new objects
     * @param reset Function to reset objects when returned to pool
     * @param allowGrowth Whether pool can grow beyond initial size
     */
    ObjectPool(size_t poolSize, std::function<std::unique_ptr<T>()> factory,
               std::function<void(T*)> reset = nullptr, bool allowGrowth = true)
        : mPoolSize(poolSize),
          mFactory(factory),
          mReset(reset),
          mAllowGrowth(allowGrowth) {
        // Pre-allocate objects
        for (size_t i = 0; i < mPoolSize; ++i) {
            auto obj = mFactory();
            mAvailable.push_back(obj.get());
            mPool.push_back(std::move(obj));
        }

        std::cout << "ObjectPool created with " << mPoolSize << " objects\n";
    }

    /**
     * @brief Acquire an object from the pool
     * @return Pointer to available object, or nullptr if pool is exhausted
     */
    T* acquire() {
        // If no available objects and growth is allowed, create more
        if (mAvailable.empty()) {
            if (mAllowGrowth) {
                auto obj = mFactory();
                T* ptr = obj.get();
                mPool.push_back(std::move(obj));
                mInUse.push_back(ptr);
                std::cout << "ObjectPool grew to " << mPool.size()
                          << " objects\n";
                return ptr;
            } else {
                std::cerr << "ObjectPool exhausted!\n";
                return nullptr;
            }
        }

        // Get object from available list
        T* obj = mAvailable.back();
        mAvailable.pop_back();
        mInUse.push_back(obj);

        return obj;
    }

    /**
     * @brief Release an object back to the pool
     * @param obj Pointer to object to release
     */
    void release(T* obj) {
        if (obj == nullptr) return;

        // Find and remove from in-use list
        auto it = std::find(mInUse.begin(), mInUse.end(), obj);
        if (it != mInUse.end()) {
            mInUse.erase(it);

            // Reset object if reset function provided
            if (mReset) {
                mReset(obj);
            }

            // Add back to available list
            mAvailable.push_back(obj);
        } else {
            std::cerr << "Attempted to release object not in pool!\n";
        }
    }

    /**
     * @brief Release all objects back to the pool
     */
    void releaseAll() {
        // Move all in-use objects to available
        for (T* obj : mInUse) {
            if (mReset) {
                mReset(obj);
            }
            mAvailable.push_back(obj);
        }
        mInUse.clear();
    }

    /**
     * @brief Get number of available objects
     */
    size_t getAvailableCount() const { return mAvailable.size(); }

    /**
     * @brief Get number of in-use objects
     */
    size_t getInUseCount() const { return mInUse.size(); }

    /**
     * @brief Get total pool size
     */
    size_t getTotalSize() const { return mPool.size(); }

    /**
     * @brief Get all in-use objects
     */
    const std::vector<T*>& getInUse() const { return mInUse; }

    /**
     * @brief Clear the entire pool
     */
    void clear() {
        mAvailable.clear();
        mInUse.clear();
        mPool.clear();
    }

    /**
     * @brief Destructor
     */
    ~ObjectPool() {
        std::cout << "ObjectPool destroyed. Total objects: " << mPool.size()
                  << "\n";
    }
};

/**
 * @brief Helper class for automatic object release (RAII pattern)
 * @tparam T Type of pooled object
 */
template <typename T>
class PooledObject {
   private:
    T* mObject;
    ObjectPool<T>* mPool;

   public:
    /**
     * @brief Constructor
     * @param object Pointer to pooled object
     * @param pool Pointer to the pool
     */
    PooledObject(T* object, ObjectPool<T>* pool)
        : mObject(object), mPool(pool) {}

    /**
     * @brief Destructor - automatically releases object back to pool
     */
    ~PooledObject() {
        if (mPool && mObject) {
            mPool->release(mObject);
        }
    }

    // Delete copy constructor and assignment
    PooledObject(const PooledObject&) = delete;
    PooledObject& operator=(const PooledObject&) = delete;

    // Allow move operations
    PooledObject(PooledObject&& other) noexcept
        : mObject(other.mObject), mPool(other.mPool) {
        other.mObject = nullptr;
        other.mPool = nullptr;
    }

    PooledObject& operator=(PooledObject&& other) noexcept {
        if (this != &other) {
            if (mPool && mObject) {
                mPool->release(mObject);
            }

            mObject = other.mObject;
            mPool = other.mPool;

            other.mObject = nullptr;
            other.mPool = nullptr;
        }
        return *this;
    }

    /**
     * @brief Access the pooled object
     */
    T* get() { return mObject; }
    const T* get() const { return mObject; }

    /**
     * @brief Pointer-like access
     */
    T* operator->() { return mObject; }
    const T* operator->() const { return mObject; }

    T& operator*() { return *mObject; }
    const T& operator*() const { return *mObject; }

    /**
     * @brief Check if object is valid
     */
    operator bool() const { return mObject != nullptr; }
};
