using System;
using ConsoleHook;
using Microsoft.Xna.Framework;
using Nebula;
using System.Reflection;
using Nebula.Game;

namespace Nebula
{
    struct PlayerData
    {
        public ComponentData<float> speed;
        public ComponentData<int> health;
        public ComponentData<Vector3> position;
    };
    
    class PlayerComponent : Component<PlayerData>
    {
        public PlayerComponent()
        {
            // Name, events and everything is derived with reflection
            Game.ComponentManager.RegisterComponent((IComponent)this);

            // Type typeinfo = typeof(T);
            // Console.WriteLine($"ComponentData: {typeinfo.FullName}:");
            // FieldInfo[] members = typeinfo.GetFields();
            // Console.WriteLine("Fields:");
            // foreach (var member in members)
            // {
            //     Console.WriteLine($"{member.DeclaringType}.{member.Name} : {member.FieldType} -- {member.Module}:{member.MetadataToken}");
            // }
        }

        public override void SetupEvents()
        {
            this.RegisterEvent(Events.OnFrame, this.OnFrame);
            // this.RegisterEvent(Events.OnActivate, this.OnActivate);
            // this.RegisterEvent(Events.OnDeactivate, this.OnDeactivate);

        }

        void OnFrame()
        {
            Console.WriteLine("OnFrame Called");
        }

        void OnActivate(InstanceId instance)
        {
            Console.WriteLine("OnActivate Called");
        }

        void OnDeactivate(InstanceId instance)
        {
            Console.WriteLine("OnActivate Called");
        }
    }

    public class AppEntry
    {
        static public void Main()
        {
            Game.Entity entity = Nebula.EntityManager.CreateEntity();
            Matrix mat = entity.Transform;

            PlayerComponent c = new PlayerComponent();
            c.Register(entity);

            Game.ComponentManager.OnFrame();
        }
    }
}