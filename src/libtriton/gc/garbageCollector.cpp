//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the BSD License.
*/

#include <algorithm>
#include <utility>

#include <triton/garbageCollector.hpp>



namespace triton {
  namespace gc {

    /* The global garbage collector instance used while the library is alive. */
    GarbageCollector* gcInstance;


    GarbageCollector::GarbageCollector() {
      this->end = false;
      #if !defined(IS_PINTOOL)
      this->t = std::thread(&GarbageCollector::threadRelease, this);
      #endif
    }


    GarbageCollector::~GarbageCollector() {
      /* Tell to the thread that we are going to dead */
      this->end = true;

      /* waits for the thread to finish its execution */
      if (this->t.joinable()) {
        t.join();
      }

      this->releaseAll();
    }


    void GarbageCollector::collect(triton::ast::AbstractNode* node) {
      #if !defined(IS_PINTOOL)
      std::list<triton::ast::SharedAbstractNode> W;

      for (auto&& n : node->getChildren()) {
        W.push_back(n);
      }

      while (!W.empty()) {
        auto& node = W.back();
        W.pop_back();

        for (auto&& n : node->getChildren()) {
          W.push_back(n);
        }

        if (node.use_count() == 1) {
          this->m1.lock();
          this->nodes.insert(node);
          this->m1.unlock();
        }
      }
      #endif
    }


    void GarbageCollector::collect(triton::engines::symbolic::SymbolicExpression* expr) {
      #if !defined(IS_PINTOOL)
      std::list<triton::ast::SharedAbstractNode> W{expr->getAst()};

      while (!W.empty()) {
        auto& node = W.back();
        W.pop_back();

        for (auto&& n : node->getChildren())
          W.push_back(n);

        if (node->getType() == triton::ast::REFERENCE_NODE) {
          const auto& expr = reinterpret_cast<triton::ast::ReferenceNode*>(node.get())->getSymbolicExpression();
          if (expr.use_count() == 1) {
            this->m2.lock();
            this->expressions.insert(expr);
            this->m2.unlock();
          }
        }
      }
      #endif
    }


    void GarbageCollector::threadRelease(void) {
      /* This loop is processed in a thread while GarbageCollector is alive */
      while (this->end == false) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->releaseAll();
      }
    }


    void GarbageCollector::releaseAll(void) {
      bool stop = false;

      /*
       * This part of the code is processed in order to release garbages
       * until there is nothing to release anymore.
       */
      while (stop == false) {
        stop = true;
        while (this->expressions.size()) {
          this->release();
          stop = false;
        }
        while (this->nodes.size()) {
          this->release();
          stop = false;
        }
      }
    }


    void GarbageCollector::release(void) {
      std::set<triton::ast::SharedAbstractNode> garbageNodes;
      std::set<triton::engines::symbolic::SharedSymbolicExpression> garbageExpressions;

      this->m1.lock();
      std::swap(garbageNodes, this->nodes);
      this->m1.unlock();

      this->m2.lock();
      std::swap(garbageExpressions, this->expressions);
      this->m2.unlock();

      //std::cout << "Release " << garbageNodes.size() << " nodes" << std::endl;
      //std::cout << "Release " << garbageExpressions.size() << " expressions" << std::endl;

      garbageNodes.clear();
      garbageExpressions.clear();
    }

  }; /* gc namespace */
}; /* triton namespace */
