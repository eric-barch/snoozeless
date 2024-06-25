export const getUnixTimeService = async () => {
  return Math.floor(Date.now() / 1000);
};
