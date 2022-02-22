import 'package:snoozeless/devicesList/devices_list.dart';
import 'package:snoozeless/home/home.dart';
import 'package:snoozeless/login/login.dart';
import 'package:snoozeless/profile/profile.dart';

var appRoutes = {
  '/login': (context) => const LoginScreen(),
  '/': (context) => const HomeScreen(),
  '/devices': (context) => const DevicesList(),
  '/profile': (context) => const ProfileScreen(),
};
