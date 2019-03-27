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

            Game.Entity entity = Nebula.EntityManager.CreateEntity();
            Matrix mat = entity.GetTransform(entity);
            Console.WriteLine("entity = {0}", entity);
            Console.WriteLine("matrix = {0}", mat);
        }
    }
}