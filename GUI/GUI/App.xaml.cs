
using System.Windows;



namespace GUI
{
    public partial class App : Application
    {
        void App_Startup(object sender, StartupEventArgs a)
        {
            /*Communicator communicator = new Communicator();
            if (communicator == null)
            {
                throw new System.Exception("Communicator is null");
            }*/
            MainWindow mainWindow = new MainWindow(/*communicator*/);
            mainWindow.Show();
        }
    }
}
