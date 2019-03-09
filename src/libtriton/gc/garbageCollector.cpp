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
    GarbageCollector gcInstance;


    GarbageCollector::GarbageCollector() {
    }


    GarbageCollector::~GarbageCollector() {
      this->loopRelease();
    }


    void GarbageCollector::collect(triton::ast::AbstractNode* node) {
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
          this->nodes.insert(node);
        }
      }
    }


    void GarbageCollector::collect(triton::engines::symbolic::SymbolicExpression* expr) {
      std::list<triton::ast::SharedAbstractNode> W{std::move(expr->getAst())};

      while (!W.empty()) {
        auto& node = W.back();
        W.pop_back();

        for (auto&& n : node->getChildren())
          W.push_back(n);

        if (node->getType() == triton::ast::REFERENCE_NODE) {
          const auto& expr = reinterpret_cast<triton::ast::ReferenceNode*>(node.get())->getSymbolicExpression();
          if (expr.use_count() == 1) {
            this->expressions.insert(expr);
          }
        }
      }
    }


    void GarbageCollector::loopRelease(void) {
      bool stop = false;

      /* Garbage collect while it's needed */
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

      std::swap(garbageNodes, this->nodes);
      std::swap(garbageExpressions, this->expressions);

      //std::cout << "Release " << garbageNodes.size() << " nodes" << std::endl;
      //std::cout << "Release " << garbageExpressions.size() << " expressions" << std::endl;

      garbageNodes.clear();
      garbageExpressions.clear();
    }

  }; /* gc namespace */
}; /* triton namespace */
