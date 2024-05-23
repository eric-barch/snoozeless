// Custom packages
import 'package:snoozeless/home/home.dart';
import 'package:snoozeless/devices/devices_list_screen/devices_list_screen.dart';
import 'package:snoozeless/login/login.dart';
import 'package:snoozeless/profile/profile.dart';

var appRoutes = {
  '/': (context) => const HomeScreen(),
  '/devices': (context) => const DevicesListScreen(),
  '/login': (context) => const LoginScreen(),
  '/profile': (context) => const ProfileScreen(),
};
