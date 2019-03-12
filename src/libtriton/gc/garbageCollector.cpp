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

    GarbageCollector::GarbageCollector() {
      this->limit = 1000;
      this->threadAllowed = false;
    }


    GarbageCollector::~GarbageCollector() {
      if (this->t.joinable()) {
        this->t.join();
      }
      this->releaseAll();
    }


    void GarbageCollector::collect(triton::ast::AbstractNode* node) {
      std::list<triton::ast::SharedAbstractNode> W;

      for (auto&& n : node->getChildren()) {
        W.push_back(n);
      }

      /* Collect garbages */
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

      /* Release garbages */
      if ((this->expressions.size() >= this->limit || this->nodes.size() >= this->limit) && this->t.joinable() == false) {
        if (this->threadAllowed == true) {
          this->t = std::thread(&GarbageCollector::releaseAll, this);
        }
        else {
          this->releaseAll();
        }
      }
    }


    void GarbageCollector::collect(triton::engines::symbolic::SymbolicExpression* expr) {
      std::list<triton::ast::SharedAbstractNode> W{expr->getAst()};

      /* Collect garbages */
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

      /* Release garbages */
      if ((this->expressions.size() >= this->limit || this->nodes.size() >= this->limit) && this->t.joinable() == false) {
        if (this->threadAllowed == true) {
          this->t = std::thread(&GarbageCollector::releaseAll, this);
        }
        else {
          this->releaseAll();
        }
      }
    }


    void GarbageCollector::releaseAll(void) {
      /*
       * This part of the code is processed in order to release garbages
       * until there is nothing to release anymore.
       */
      while (this->release());
    }


    bool GarbageCollector::release(void) {
      std::set<triton::ast::SharedAbstractNode> garbageNodes;
      std::set<triton::engines::symbolic::SharedSymbolicExpression> garbageExpressions;
      bool released = false;

      this->m1.lock();
      std::swap(garbageNodes, this->nodes);
      this->m1.unlock();

      this->m2.lock();
      std::swap(garbageExpressions, this->expressions);
      this->m2.unlock();

      //std::cout << "Release " << garbageNodes.size() << " nodes" << std::endl;
      //std::cout << "Release " << garbageExpressions.size() << " expressions" << std::endl;

      released = (garbageNodes.size() || garbageExpressions.size());

      garbageNodes.clear();
      garbageExpressions.clear();

      return released;
    }


    void GarbageCollector::allowThread(bool state) {
      this->threadAllowed = state;
    }

  }; /* gc namespace */
}; /* triton namespace */
