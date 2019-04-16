using System;
using ConsoleHook;
using Microsoft.Xna.Framework;
using Nebula;
using System.Reflection;

namespace Nebula
{
    struct PlayerData
    {
        public Vector3 position;
        public float speed;
        public int health;
    };
    
    class Component<T>
    {
        public void Create()
        {
            Type typeinfo = typeof(T);
            Console.WriteLine($"ComponentData: {typeinfo.FullName}:");
            FieldInfo[] members = typeinfo.GetFields();
            Console.WriteLine("Fields:");
            foreach (var member in members)
            {
                Console.WriteLine($"{member.DeclaringType}.{member.Name} : {member.FieldType} -- {member.Module}:{member.MetadataToken}");
            }
            // Interop.SetupComponent()
        }
    }

    public class AppEntry
    {
        static public void Main()
        {
            Game.Entity entity = Nebula.EntityManager.CreateEntity();
            Matrix mat = entity.Transform;

            Component<PlayerData> c = new Component<PlayerData>();
            c.Create();
        }
    }
}