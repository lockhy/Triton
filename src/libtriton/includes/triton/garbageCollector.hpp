//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the BSD License.
*/

#ifndef TRITON_GARBAGE_COLLECTOR_HPP
#define TRITON_GARBAGE_COLLECTOR_HPP

#include <set>

#include <triton/ast.hpp>
#include <triton/dllexport.hpp>
#include <triton/symbolicExpression.hpp>



//! The Triton namespace
namespace triton {
/*!
 *  \addtogroup triton
 *  @{
 */

  //! The Garbage Collector namespace
  namespace gc {
  /*!
   *  \ingroup triton
   *  \addtogroup gc
   *  @{
   */

    //! \class GarbageCollector
    /*! \brief This class is used to garbage collector unused AbstractNode and SymbolicExpressions. */
    class GarbageCollector {
      public:
        //! The list of AbstractNode which must be deleted.
        std::set<triton::ast::SharedAbstractNode> nodes;

        //! The list of SymbolicExpression which must be deleted.
        std::set<triton::engines::symbolic::SharedSymbolicExpression> expressions;

        //! Constructor
        TRITON_EXPORT GarbageCollector();

        //! Destructor
        TRITON_EXPORT ~GarbageCollector();

        //! Collects an AbstractNode
        TRITON_EXPORT void collect(triton::ast::AbstractNode* node);

        //! Collects an SymbolicExpression
        TRITON_EXPORT void collect(triton::engines::symbolic::SymbolicExpression* expr);

        //! Releases garbages. It will free unused AbstractNode and SymbolicExpression.
        TRITON_EXPORT void release(void);
    };

    //! The global garbage collector instance used while the library is alive.
    extern GarbageCollector gcInstance;

  /*! @} End of gc namespace */
  };
/*! @} End of triton namespace */
};

#endif /* TRITON_GARBAGE_COLLECTOR_HPP */
