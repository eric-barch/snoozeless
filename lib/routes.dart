// Custom packages
import 'package:snoozeless/home/home.dart';
import 'package:snoozeless/devices_screen/devices_screen.dart';
import 'package:snoozeless/login/login.dart';
import 'package:snoozeless/profile/profile.dart';

var appRoutes = {
  '/': (context) => const HomeScreen(),
  '/devices': (context) => const DevicesScreen(),
  '/login': (context) => const LoginScreen(),
  '/profile': (context) => const ProfileScreen(),
};
