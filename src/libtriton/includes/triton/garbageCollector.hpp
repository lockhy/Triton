//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the BSD License.
*/

#ifndef TRITON_GARBAGE_COLLECTOR_HPP
#define TRITON_GARBAGE_COLLECTOR_HPP

#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

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
      private:
        //! Internal mutex used for nodes.
        std::mutex m1;

        //! Internal mutex used for expressions.
        std::mutex m2;

        //! Internal thread to release garbages.
        std::thread t;

        //! This flag is false while GarbageCollector is alive. ~GarbageCollector sets this flag to true.
        bool end;

        //! Releases garbages in a thread while GarbageCollector is alive.
        void threadRelease(void);

      public:
        //! The list of AbstractNode which must be deleted.
        std::set<triton::ast::SharedAbstractNode> nodes;

        //! The list of SymbolicExpression which must be deleted.
        std::set<triton::engines::symbolic::SharedSymbolicExpression> expressions;

        //! Constructor.
        TRITON_EXPORT GarbageCollector();

        //! Destructor.
        TRITON_EXPORT ~GarbageCollector();

        //! Collects an AbstractNode.
        TRITON_EXPORT void collect(triton::ast::AbstractNode* node);

        //! Collects an SymbolicExpression.
        TRITON_EXPORT void collect(triton::engines::symbolic::SymbolicExpression* expr);

        //! Release garbages until there is nothing to release anymore.
        TRITON_EXPORT void releaseAll(void);

        //! Releases the current collected garbages.
        TRITON_EXPORT void release(void);
    };

    //! Shared Garbage Collector
    using SharedGarbageCollector = std::shared_ptr<triton::gc::GarbageCollector>;

  /*! @} End of gc namespace */
  };
/*! @} End of triton namespace */
};

#endif /* TRITON_GARBAGE_COLLECTOR_HPP */
