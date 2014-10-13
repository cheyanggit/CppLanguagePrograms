// ***********************************************************************
// Filename         : ScopeExitGuard.hpp
// Author           : LIZHENG
// Created          : 2014-10-01
// Description      : RAII�࣬������Դ�ͷš�����
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-10-01
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SCOPEEXITGUARD_H
#define ZL_SCOPEEXITGUARD_H
#include <stdio.h>
#include <Windows.h>

namespace detail
{
	// Release algorithms (release policies)
	template < typename T >
	struct DeletePolicy
	{
		void operator()(T handle)
		{
			delete (handle);
		}
	};

	template <>
	struct DeletePolicy<FILE*>
	{
		void operator()(FILE *handle)
		{
			fclose(handle);
		}
	};

	template < typename T >
	struct FreeMemPolicy
	{
		void operator()(T handle)
		{
			free(handle);
		}
	}; 

	template < typename T >
	struct FreeLibraryPolicy
	{
		void operator()(T handle)
		{
			BOOL suc = ::FreeLibrary(handle);
			assert(suc);
		}
	}; 
}

template <typename HandleT,
         template< typename > class ReleasePolicyT = detail::DeletePolicy,
         HandleT NULL_VALUE = 0
         >
class ScopeExitGuard : private ReleasePolicyT<HandleT>
{
    typedef HandleT	value_type;

public:
    ScopeExitGuard() : handle_(NULL_VALUE)
    {
    }

    ScopeExitGuard(const value_type& h) : handle_(h)
    {
    }

    ScopeExitGuard(const ScopeExitGuard& h)
    {
        Cleanup();
        handle_ = h.handle_;
    }

    ~ScopeExitGuard()
    {
        Cleanup();
    }

    ScopeExitGuard& operator=(const ScopeExitGuard& rhs)
    {
        if(&rhs != this)
        {
            Cleanup();
            handle_ = rhs.handle_;
        }
        return (*this);
    }

    value_type& operator=(const value_type& hande)
    {
        if(hande != handle_)
        {
            Cleanup();
            handle_ = hande;
        }
        return handle_;
    }

    value_type& Get()
    {
        return handle_;
    }

    const value_type& Get() const
    {
        return handle_;
    }

    bool IsValid() const
    {
        return handle_ != NULL_VALUE;
    }

    operator bool() const
    {
        return IsValid();
    }

    value_type Detach()
    {
        value_type hHandle = handle_;
        handle_ = NULL_VALUE;
        return hHandle;
    }

    void Cleanup()
    {
        if (handle_ != NULL_VALUE)
        {
            operator()(handle_);
            handle_ = NULL_VALUE;
        }
    }

private:
	value_type handle_;
};


typedef ScopeExitGuard<FILE*,	detail::FreeMemPolicy>						    ScopeFreeMem;
typedef ScopeExitGuard<FILE*,	detail::DeletePolicy>						    ScopeCloseFile;
typedef ScopeExitGuard<HMODULE,	detail::FreeLibraryPolicy>						ScopeFreeLibrary;


#endif  /* ZL_SCOPEEXITGUARD_H */