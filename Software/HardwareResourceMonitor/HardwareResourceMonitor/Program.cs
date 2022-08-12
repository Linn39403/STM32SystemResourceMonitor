using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using OpenHardwareMonitor.Hardware;
using System.IO.Ports;

namespace HardwareResourceMonitor
{
    class Program
    {
        static SerialPort _serialPort;
        public class UpdateVisitor : IVisitor
        {
            public void VisitComputer(IComputer computer)
            {
                computer.Traverse(this);
            }
            public void VisitHardware(IHardware hardware)
            {
                hardware.Update();
                foreach (IHardware subHardware in hardware.SubHardware) subHardware.Accept(this);
            }
            public void VisitSensor(ISensor sensor) { }
            public void VisitParameter(IParameter parameter) { }
        }
        static void GetSystemInfo()
        {
            UpdateVisitor updateVisitor = new UpdateVisitor();
            Computer computer = new Computer();
            computer.Open();
            computer.CPUEnabled = true;
            computer.GPUEnabled = true;
            computer.RAMEnabled = true;
            computer.Accept(updateVisitor);
            string CPULoadString = "";
            string GPULoadString = "";
            string CPUTempString = "";
            string GPUTempString = "";
            string RAMInfoString = "";
            string TimeString = "";
            string SystemInfoString = "";
            for (int i = 0; i < computer.Hardware.Length; i++)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.CPU)
                {
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature &&
                            computer.Hardware[i].Sensors[j].Name == "CPU Package")
                        {
                            CPUTempString =  "CPUTemp:" + computer.Hardware[i].Sensors[j].Value.ToString() ;
                        }
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Load &&
                            computer.Hardware[i].Sensors[j].Name == "CPU Total")
                        {
                            CPULoadString = "CPULoad:" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                    }
                }
                if(computer.Hardware[i].HardwareType == HardwareType.GpuNvidia)
                {
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature &&
                            computer.Hardware[i].Sensors[j].Name == "GPU Core")
                        {
                            GPUTempString = "GPUTemp:" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Load &&
                            computer.Hardware[i].Sensors[j].Name == "GPU Core")
                        {
                            GPULoadString = "GPULoad:" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                    }
                }
                if (computer.Hardware[i].HardwareType == HardwareType.RAM)
                {
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Load &&
                            computer.Hardware[i].Sensors[j].Name == "Memory")
                        {
                            RAMInfoString ="RAM:" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                    }
                }
            }
            TimeString = "Time:" + DateTime.Now.ToString("h-mm-ss");
            SystemInfoString = CPUTempString + "," + CPULoadString + "," + GPUTempString + "," + GPULoadString + "," + RAMInfoString + "," + TimeString;
            Console.WriteLine(SystemInfoString);
            _serialPort.WriteLine(SystemInfoString);
            computer.Close();
        }
        static void Main(string[] args)
        {
            //Temp Choose COM7
            _serialPort = new SerialPort();
            _serialPort.PortName = "COM9";
            _serialPort.BaudRate = 115200;
            _serialPort.DataBits = 8;
            _serialPort.Parity = Parity.None;
            _serialPort.StopBits = StopBits.One;
            _serialPort.Open();
            while (true)
            {
                GetSystemInfo();
                Thread.Sleep(1000);
            }
            _serialPort.Close();
        }
    }
}
