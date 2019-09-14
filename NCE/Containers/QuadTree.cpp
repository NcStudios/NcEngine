#include "QuadTree.hpp"

namespace NCE::Containers
{

    QuadTree::QuadTree(int t_maxDensity, NCE::Common::Vector4 t_localQuad)
    {
        _localQuad = t_localQuad;
        _maxDensity = t_maxDensity;
        _containedElements.reserve(_maxDensity);
    }

    void QuadTree::Partition()
    {
        _isPartitioned = true;

        NCE::Common::Vector4 quadArea( _localQuad.GetA() + _localQuad.GetC() / 2, //this.x + this.width  / 2 = new.x
                                       _localQuad.GetB(),                         //this.y = new.y
                                       _localQuad.GetC() / 2,                     //this.width  / 2 = new.width
                                       _localQuad.GetD() / 2);                    //this.height / 2 = new.height

        _childQuad1 = std::make_unique<QuadTree>(_maxDensity, quadArea);


        quadArea.Set( _localQuad.GetA(),      //this.x = new.x
                      _localQuad.GetB(),      //this.y = new.y
                      _localQuad.GetC() / 2,  //this.width  / 2 = new.width
                      _localQuad.GetD() / 2); //this.height / 2 = new.height

        _childQuad2 = std::make_unique<QuadTree>(_maxDensity, quadArea);


        quadArea.Set( _localQuad.GetA(),                         //this.x = new.x
                      _localQuad.GetB() + _localQuad.GetD() / 2, //this.y + this.height / 2 = new.y
                      _localQuad.GetC() / 2,                     //this.width  / 2 = new.width
                      _localQuad.GetD() / 2);                    //this.height / 2 = new.height

        _childQuad3 = std::make_unique<QuadTree>(_maxDensity, quadArea);


        quadArea.Set( _localQuad.GetA() + _localQuad.GetC() / 2, //this.x + this.width  / 2 = new.x
                      _localQuad.GetB() + _localQuad.GetD() / 2, //this.y + this.height / 2 = new.y
                      _localQuad.GetC() / 2,                     //this.width  / 2 = new.width
                      _localQuad.GetD() / 2);                    //this.height / 2 = new.height

        _childQuad4 = std::make_unique<QuadTree>(_maxDensity, quadArea);
    }

    bool QuadTree::AreIntersecting(const NCE::Common::Vector4 &t_first, const NCE::Common::Vector4 &t_second)
    {
        if ((  t_first.GetA() > (t_second.GetA() + t_second.GetC()) ) || //first.left > second.right
            (  t_first.GetB() > (t_second.GetB() + t_second.GetD()) ) || //first.top  > second.bottom
            ( (t_first.GetA() +  t_first.GetC()) < t_second.GetA()  ) || //first.right < second.left
            ( (t_first.GetB() +  t_first.GetD()) < t_second.GetB()  ))  //first.bottom < second.bottom

        {
            return false;
        }

        return true;
    }

    void QuadTree::AddElement(std::weak_ptr<NCE::Components::Collider> t_newElement)
    {
        if (!AreIntersecting(_localQuad, t_newElement.lock()->GetRect()))
        {
            return;
        }

        if (_isPartitioned)
        {
            _childQuad1->AddElement(t_newElement);
            _childQuad2->AddElement(t_newElement);
            _childQuad3->AddElement(t_newElement);
            _childQuad4->AddElement(t_newElement);
            return;
        }

        if (_currentDensity < _maxDensity)
        {
            _containedElements.push_back(t_newElement);
            ++_currentDensity;
            return;
        }

        Partition();

        for(auto& existingElement : _containedElements)
        {
            _childQuad1->AddElement(existingElement);
            _childQuad2->AddElement(existingElement);
            _childQuad3->AddElement(existingElement);
            _childQuad4->AddElement(existingElement);
        }

        _containedElements.clear();

        _childQuad1->AddElement(t_newElement);
        _childQuad2->AddElement(t_newElement);
        _childQuad3->AddElement(t_newElement);
        _childQuad4->AddElement(t_newElement);
    }

    void QuadTree::CheckCollisions()
    {
        if (_isPartitioned)
        {
            _childQuad1->CheckCollisions();
            _childQuad2->CheckCollisions();
            _childQuad3->CheckCollisions();
            _childQuad4->CheckCollisions();
            return;
        }

        for (auto& collider : _containedElements)
        {
            for (auto& other : _containedElements)
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

        //std::vector<std::weak_ptr<NCE::Components::Collider>>::iterator i;
        //std::vector<std::weak_ptr<NCE::Components::Collider>>::iterator j;

        // for (auto i = _containedElements.begin(); i != _containedElements.end(); ++i)
        // {
        //     for (auto j = i + 1; j != _containedElements.end(); ++j)
        //     {
        //         if (i->lock() == j->lock())
        //         {
        //             continue;
        //             std::cout << "**shouldn't ever be printed**" << '\n';
        //         }

        //         else if (AreIntersecting(i->lock()->GetRect(), j->lock()->GetRect()))
        //         {
        //             i->lock()->RegisterCollisionEvent(j);
        //         }
        //     }
        // }

    } //end CheckCollisions()

    void QuadTree::Clear()
    {
        _currentDensity = 0;
        _containedElements.clear();
        _isPartitioned = false;
        _childQuad1 = nullptr;
        _childQuad2 = nullptr;
        _childQuad3 = nullptr;
        _childQuad4 = nullptr;
    }
}