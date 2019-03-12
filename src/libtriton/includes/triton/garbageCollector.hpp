//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the BSD License.
*/

#ifndef TRITON_GARBAGE_COLLECTOR_HPP
#define TRITON_GARBAGE_COLLECTOR_HPP

#include <atomic>
#include <chrono>
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
        std::recursive_mutex m1;

        //! Internal mutex used for expressions.
        std::recursive_mutex m2;

        //! Internal thread to release garbages.
        std::thread t;

        //! The limit of gargabes until the release of them.
        triton::uint32 limit;

        //! True if threads are allowed. Some DBI like Pin does not like threads...
        bool threadAllowed;

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

        //! Releases garbages until there is nothing to release anymore.
        TRITON_EXPORT void releaseAll(void);

        //! Releases the current collected garbages. Returns true if garbages were been released.
        TRITON_EXPORT bool release(void);

        //! Allows threads
        TRITON_EXPORT void allowThread(bool state);
    };

  /*! @} End of gc namespace */
  };
/*! @} End of triton namespace */
};

#endif /* TRITON_GARBAGE_COLLECTOR_HPP */
