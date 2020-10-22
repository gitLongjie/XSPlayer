#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>

namespace XSPlayer {
template <class T>
class Singleton {
  public:
    Singleton( void ) {
        assert( !m_pSingleton );
        m_pSingleton = static_cast< T* >( this );
    }
    virtual ~Singleton( void ) {
        assert( m_pSingleton );
        m_pSingleton = 0;
    }

    static T& GetSingleton( void ) {
        assert( m_pSingleton );
        return ( *m_pSingleton );
    }
    static T* GetSingletonPtr( void ) {
        return m_pSingleton;
    }

  protected:
    static T* m_pSingleton;
};
}

#endif // SINGLETON_H
