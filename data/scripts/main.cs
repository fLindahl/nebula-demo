using System;
using ConsoleHook;
using Microsoft.Xna.Framework;
using Nebula;

namespace Nebula
{
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

            Console.WriteLine("entity = ghdsgashgsdhgasogaog");
            Game.Entity entity = Nebula.EntityManager.CreateEntity();
            Console.WriteLine("entity = {0}", entity);
            Matrix m = entity.GetTransform(entity);
            Console.WriteLine("matrix = {0}", m);
        }
    }
}