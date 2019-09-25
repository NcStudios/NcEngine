#include "QuadTree.hpp"

namespace NCE::Containers
{

    QuadTree::QuadTree(int maxDensity_, NCE::Common::Vector4 area_)
    {
        m_area = area_;
        m_maxDensity = maxDensity_;
        m_containedElements.reserve(m_maxDensity);
    }

    void QuadTree::Partition()
    {
        m_isPartitioned = true;

        NCE::Common::Vector4 subRect( m_area.GetX() + m_area.GetZ() / 2, //this.x + this.width  / 2 = new.x
                                      m_area.GetY(),                     //this.y = new.y
                                      m_area.GetZ() / 2,                 //this.width  / 2 = new.width
                                      m_area.GetW() / 2);                //this.height / 2 = new.height

        m_childQuad1 = std::make_unique<QuadTree>(m_maxDensity, subRect);


        subRect.Set( m_area.GetX(),      //this.x = new.x
                     m_area.GetY(),      //this.y = new.y
                     m_area.GetZ() / 2,  //this.width  / 2 = new.width
                     m_area.GetW() / 2); //this.height / 2 = new.height

        m_childQuad2 = std::make_unique<QuadTree>(m_maxDensity, subRect);


        subRect.Set( m_area.GetX(),                          //this.x = new.x
                     m_area.GetY() + m_area.GetW() / 2, //this.y + this.height / 2 = new.y
                     m_area.GetZ() / 2,                      //this.width  / 2 = new.width
                     m_area.GetW() / 2);                     //this.height / 2 = new.height

        m_childQuad3 = std::make_unique<QuadTree>(m_maxDensity, subRect);


        subRect.Set( m_area.GetX() + m_area.GetZ() / 2, //this.x + this.width  / 2 = new.x
                     m_area.GetY() + m_area.GetW() / 2, //this.y + this.height / 2 = new.y
                     m_area.GetZ() / 2,                      //this.width  / 2 = new.width
                     m_area.GetW() / 2);                     //this.height / 2 = new.height

        m_childQuad4 = std::make_unique<QuadTree>(m_maxDensity, subRect);
    }

    bool QuadTree::AreIntersecting(const NCE::Common::Vector4 &first_, const NCE::Common::Vector4 &second_)
    {
        if ((  first_.GetX() > (second_.GetX() + second_.GetZ()) ) || //first.left > second.right
            (  first_.GetY() > (second_.GetY() + second_.GetW()) ) || //first.top  > second.bottom
            ( (first_.GetX() +  first_.GetZ()) < second_.GetX()  ) || //first.right < second.left
            ( (first_.GetY() +  first_.GetW()) < second_.GetY()  ))   //first.bottom < second.bottom

        {
            return false;
        }

        return true;
    }

    void QuadTree::AddElement(std::weak_ptr<NCE::Components::Collider> newElement_)
    {
        if (!AreIntersecting(m_area, newElement_.lock()->GetRect()))
        {
            return;
        }

        if (m_isPartitioned)
        {
            m_childQuad1->AddElement(newElement_);
            m_childQuad2->AddElement(newElement_);
            m_childQuad3->AddElement(newElement_);
            m_childQuad4->AddElement(newElement_);
            return;
        }

        if (m_currentDensity < m_maxDensity)
        {
            m_containedElements.push_back(newElement_);
            ++m_currentDensity;
            return;
        }

        Partition();

        for(auto& existingElement : m_containedElements)
        {
            m_childQuad1->AddElement(existingElement);
            m_childQuad2->AddElement(existingElement);
            m_childQuad3->AddElement(existingElement);
            m_childQuad4->AddElement(existingElement);
        }

        m_containedElements.clear();

        m_childQuad1->AddElement(newElement_);
        m_childQuad2->AddElement(newElement_);
        m_childQuad3->AddElement(newElement_);
        m_childQuad4->AddElement(newElement_);
    }

    void QuadTree::CheckCollisions()
    {
        if (m_isPartitioned)
        {
            m_childQuad1->CheckCollisions();
            m_childQuad2->CheckCollisions();
            m_childQuad3->CheckCollisions();
            m_childQuad4->CheckCollisions();
            return;
        }

        for (auto& collider : m_containedElements)
        {
            for (auto& other : m_containedElements)
            {
                if (collider.lock() == other.lock())
                {
                    continue;
                }

                if (AreIntersecting(collider.lock()->GetRect(), other.lock()->GetRect()))
                {
                    collider.lock()->RegisterCollisionEvent(other);
                }
            }
        }
    } //end CheckCollisions()

    void QuadTree::Clear()
    {
        m_currentDensity = 0;
        m_containedElements.clear();
        m_isPartitioned = false;
        m_childQuad1 = nullptr;
        m_childQuad2 = nullptr;
        m_childQuad3 = nullptr;
        m_childQuad4 = nullptr;
    }
}