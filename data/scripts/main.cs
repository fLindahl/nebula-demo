using System;
using ConsoleHook;
using Microsoft.Xna.Framework;
using Nebula;
using System.Reflection;

namespace Nebula
{
    struct PlayerData
    {
        Vector3 position;
        float speed;
        int health;
    };
    
    class Component<T>
    {
        public void Create()
        {
            Type typeinfo = typeof(T);
            MemberInfo[] members = typeinfo.FindMembers(MemberTypes.Field, BindingFlags.Default, Type.FilterName, "ReferenceEquals");
            foreach (var member in members)
            {
                Console.Write(member.Name.ToString());
                Console.Write(" : ");
                Console.WriteLine(member.ReflectedType.ToString());
            }
            // Interop.SetupComponent()
        }
    }

    public class AppEntry
    {
        static public void Main()
        {
            // Setup console redirect / log hook
            using (var consoleWriter = new ConsoleWriter())
            {
                consoleWriter.WriteEvent += ConsoleEvents.WriteFunc;
                consoleWriter.WriteLineEvent += ConsoleEvents.WriteLineFunc;
                Console.SetOut(consoleWriter);
            }

            Game.Entity entity = Nebula.EntityManager.CreateEntity();
            Matrix mat = entity.Transform;

            Component<PlayerData> c = new Component<PlayerData>();
            Console.WriteLine("here");
            c.Create();
        }
    }
}